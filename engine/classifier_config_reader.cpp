// Copyright (C) 2012-2013 Yuri Agafonov
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "stdafx.h"

#include <sstream>

#include "classifier_config_reader.h"
#include <file_stream.h>
#include <string_util.h>
#include <libxml_util.h>
#include <formatted_exception.hpp>
#include <dumbassert.h>
#include <trace.h>

class config_error : public formatted_exception<config_error> {
public:
	config_error(char const *s_ = NULL) : formatted_exception<config_error>(s_)
	{}

	virtual char const* what() const {
		char const *s_ = formatted_exception<config_error>::what();
		if (*s_)
			return s_;
		else
			return "config_error";
	}
};

inline int CharValue(char c) {
	if ('A' <= c && c <= 'Z')
		return c - 'A' + 10;
	if ('a' <= c && c <= 'z')
		return c - 'a' + 10;
	if ('0' <= c && c <= '9')
		return c - '0';
	return -1;
}
template<size_t N>
inline void ReadBytes(cpcl::StringPiece const &s, unsigned char (&r)[N]) {
	size_t n(0),nibble(0);
	unsigned int l_nibble, h_nibble;
	for (size_t i = 0; i < s.size() && n < N; ++i) {
		int v = CharValue(s[i]);
		if (v < 0)
			continue;

		if (nibble++)
			l_nibble = (unsigned int)v;
		else
			h_nibble = (unsigned int)v;

		if (nibble > 1) {
			r[n++] = (h_nibble << 4) + l_nibble;
			nibble = 0;
		}
	}
	if (n != N) {
		config_error::throw_formatted(config_error(),
			"ConfigReader()::Read(): unsupported uuid format: '%s'",
			s.as_string().c_str());
	}
}
inline GUID ReadUuid(cpcl::StringPiece const &s) {
	GUID r;
	unsigned char r_bytes[0x10/*sizeof(GUID)*/];
	ReadBytes(s, r_bytes);
	{
		r.Data1 = static_cast<unsigned long>(r_bytes[3])
			| (static_cast<unsigned long>(r_bytes[0]) << 24)
			| (static_cast<unsigned long>(r_bytes[1]) << 16)
			| (static_cast<unsigned long>(r_bytes[2]) << 8);
	}
	{
		r.Data2 = static_cast<unsigned long>(r_bytes[5])
			| (static_cast<unsigned long>(r_bytes[4]) << 8);
	}
	{
		r.Data3 = static_cast<unsigned long>(r_bytes[7])
			| (static_cast<unsigned long>(r_bytes[6]) << 8);
	}
	{
		memcpy(r.Data4, r_bytes + 8, sizeof(r.Data4));
	}
	return r;
}

static inline void ToASCIIlower(char *s) {
	for (; *s; ++s) {
		if ('A' <= *s && *s <= 'Z')
			*s = 'a' + *s - 'A';
	}
}

struct ConfigReader {
	typedef void (ConfigReader::*Handler)(cpcl::XmlReader *reader);
	typedef std::map<std::string, Handler> Handlers;
	struct MustCall {
		std::string path;
		std::map<std::string, std::string> debug_tags;
		Handlers handlers; // local handlers
		Handler at_tag_exit;

		MustCall() : at_tag_exit(0)
		{}

		void InvokeHandler(std::string const &tag_path, ConfigReader *config_reader, cpcl::XmlReader *reader) {
			Handlers::iterator i = handlers.find(tag_path);
			if (i != handlers.end()) {
				(config_reader->*(*i).second)(reader);
				handlers.erase(i);

				std::map<std::string, std::string>::iterator j = debug_tags.find(tag_path);
				debug_tags.erase(j);
			}
		}
		void RegisterHandler(cpcl::StringPiece const &path_, cpcl::StringPiece const &debug_tag, Handler handler) {
			std::string tag_path(path);
			tag_path.append(path_.data(), path_.size());
			
			handlers.insert(std::make_pair(tag_path, handler));
			debug_tags.insert(std::make_pair(tag_path, debug_tag.as_string()));
		}
		void Release(ConfigReader *config_reader, cpcl::XmlReader *reader) {
			if (!handlers.empty()) {
				std::stringstream s;
				if (!debug_tags.empty()) {
					std::map<std::string, std::string>::iterator i = debug_tags.begin();
					for (std::map<std::string, std::string>::iterator tail = --debug_tags.end(); i != tail; ++i)
						s << "<" << (*i).second << ">, ";
					s << "<" << (*i).second << ">";
				} else
					s << " ";

				config_error::throw_formatted(config_error(),
					"ConfigReader()::Read(): invalid xml structure: %s tags missed for <%s>",
					s.str().c_str(), reader->Name_UTF8().as_string().c_str());
			}
			if (at_tag_exit)
				(config_reader->*at_tag_exit)(reader);
		}
	};

	Handlers handlers; // global handlers
	std::list<MustCall> must_call;
	cpcl::WStringPiece params_path;
	std::list<DetectorInfo> detectors;

	explicit ConfigReader(cpcl::WStringPiece const &params_path_) : params_path(params_path_), text(0), wtext(0) {
		handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetector").as_string(),
			&ConfigReader::VisitDetector));
		handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetectorattributesattribute").as_string(),
			&ConfigReader::VisitAttribute));
		handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetectorattributesattributeclassesclass").as_string(),
			&ConfigReader::VisitAttributeClass));

		/*handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetectorface").as_string(),
			&ConfigReader::SetFaceDetector));*/
		handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetectorobject").as_string(),
			&ConfigReader::SetObjectDetector));
		handlers.insert(Handlers::value_type(cpcl::StringPieceFromLiteral("detectorsdetectorpeople").as_string(),
			&ConfigReader::SetPeopleDetector));
	}

	void InvokeHandlers(std::string const &tag_path, cpcl::XmlReader *reader) {
		Handlers::iterator i = handlers.find(tag_path);
		if (i != handlers.end()) {
			(this->*(*i).second)(reader);
		}
		// must_call.top().InvokeHandler(tag_path, this, reader);
		for (std::list<MustCall>::iterator i = must_call.begin(); i != must_call.end(); ++i)
			(*i).InvokeHandler(tag_path, this, reader);
	}
	void RegisterMustCall(cpcl::StringPiece const &tag_path, cpcl::StringPiece const &debug_tag, Handler handler) {
		DUMBASS_CHECK(!must_call.empty());

		// must_call.top().RegisterHandler(tag_path, debug_tag, handler);
		must_call.back().RegisterHandler(tag_path, debug_tag, handler);
	}
	void RegisterMustCall(cpcl::StringPiece const &tag, Handler handler) {
		RegisterMustCall(tag, tag, handler);
	}
	void RegisterAtTagExit(Handler handler) {
		DUMBASS_CHECK(!must_call.empty());

		// must_call.top().at_tag_exit = handler;
		must_call.back().at_tag_exit = handler;
	}

	void VisitDetector(cpcl::XmlReader*) {
		detectors.push_back(DetectorInfo());
		detectors.back().type = DetectorInfo::FACE_DETECTOR;
		RegisterMustCall(cpcl::StringPieceFromLiteral("id"), &ConfigReader::VisitDetectorID);
		RegisterMustCall(cpcl::StringPieceFromLiteral("name"), &ConfigReader::VisitDetectorName);
		RegisterMustCall(cpcl::StringPieceFromLiteral("config"), &ConfigReader::VisitDetectorConfig);
	}
	void SetObjectDetector(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());

		detectors.back().type = DetectorInfo::OBJECT_DETECTOR;
	}
	void SetPeopleDetector(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());

		detectors.back().type = DetectorInfo::PEOPLE_DETECTOR;
	}
	std::string detector_id;
	void VisitDetectorID(cpcl::XmlReader*) {
		detector_id.clear();
		text = &detector_id;
		RegisterAtTagExit(&ConfigReader::ReadDetectorID);
	}
	void ReadDetectorID(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(text);

		if (text->empty())
			throw config_error("ConfigReader()::Read(): <detector>/<id> can't be empty");
		detectors.back().id = ReadUuid(*text);
		text = 0;
	}
	void VisitDetectorName(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());

		wtext = &detectors.back().name;
		RegisterAtTagExit(&ConfigReader::ClearTextPointers);
	}
	void VisitDetectorConfig(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());

		wtext = &detectors.back().config;
		RegisterAtTagExit(&ConfigReader::ReadDetectorConfig/*ReadConfigTag*/);
	}
	void ReadDetectorConfig(cpcl::XmlReader*) {
		*wtext = cpcl::WStringPiece(*wtext).trim(cpcl::WStringPieceFromLiteral(L" \t\r\n\\")).as_string();
		if (wtext->empty())
			throw config_error("ConfigReader()::Read(): <detector>/<config> can't be empty");
		wtext->insert(0, 1, L'\\');
		wtext->insert(0, params_path.data(), params_path.size());
		wtext = 0;
	}

	void VisitAttribute(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());

		detectors.back().attributes.push_back(AttributeInfo());
		RegisterMustCall(cpcl::StringPieceFromLiteral("id"), &ConfigReader::VisitAttributeID);
		RegisterMustCall(cpcl::StringPieceFromLiteral("name"), &ConfigReader::VisitAttributeName);
		RegisterMustCall(cpcl::StringPieceFromLiteral("config"), &ConfigReader::VisitAttributeConfig);
		// RegisterMustCall(cpcl::StringPieceFromLiteral("classesclass"), cpcl::StringPieceFromLiteral("class"), &ConfigReader::VisitAttributeClass);
	}
	std::string attribute_id;
	void VisitAttributeID(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(!detectors.back().attributes.empty());

		attribute_id.clear();
		text = &attribute_id;
		RegisterAtTagExit(&ConfigReader::ReadAttributeID);
	}
	void ReadAttributeID(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(!detectors.back().attributes.empty());
		DUMBASS_CHECK(text);

		if (text->empty())
			throw config_error("ConfigReader()::Read(): <attribute>/<id> can't be empty");
		detectors.back().attributes.back().id = ReadUuid(*text);
		text = 0;
	}
	void VisitAttributeName(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(!detectors.back().attributes.empty());

		wtext = &detectors.back().attributes.back().name;
		RegisterAtTagExit(&ConfigReader::ClearTextPointers);
	}
	void VisitAttributeConfig(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(!detectors.back().attributes.empty());

		wtext = &detectors.back().attributes.back().config;
		RegisterAtTagExit(&ConfigReader::ReadAttributeConfig/*ReadConfigTag*/);
	}
	void ReadAttributeConfig(cpcl::XmlReader*) {
		DUMBASS_CHECK(wtext);

		*wtext = cpcl::WStringPiece(*wtext).trim(cpcl::WStringPieceFromLiteral(L" \t\r\n\\")).as_string();
		if (wtext->empty())
			throw config_error("ConfigReader()::Read(): <attribute>/<config> can't be empty");
		wtext->insert(0, 1, L'\\');
		wtext->insert(0, params_path.data(), params_path.size());
		wtext = 0;
	}
	std::string attribute_class_filter;
	std::wstring attribute_class_name;
	void VisitAttributeClass(cpcl::XmlReader*) {
		RegisterMustCall(cpcl::StringPieceFromLiteral("filter"), &ConfigReader::VisitAttributeClassFilter);
		RegisterMustCall(cpcl::StringPieceFromLiteral("name"), &ConfigReader::VisitAttributeClassName);
		RegisterAtTagExit(&ConfigReader::ReadAttributeClass);
	}
	void ReadAttributeClass(cpcl::XmlReader*) {
		DUMBASS_CHECK(!detectors.empty());
		DUMBASS_CHECK(!detectors.back().attributes.empty());

		if (attribute_class_filter.empty())
			throw config_error("ConfigReader()::Read(): <class>/<filter> can't be empty");
		char const *str = attribute_class_filter.c_str();
		char *tail;
		long const filter = strtol(str, &tail, 10);
		if ((filter == LONG_MAX) || (filter == LONG_MIN) || ((filter == 0) && (tail == str)))
			throw config_error("ConfigReader()::Read(): <class>/<filter> has invalid value");

		if (attribute_class_name.empty())
			throw config_error("ConfigReader()::Read(): <class>/<name> can't be empty");

		detectors.back().attributes.back().classes.insert(std::map<int, std::wstring>::value_type(filter, attribute_class_name));
	}
	void VisitAttributeClassFilter(cpcl::XmlReader*) {
		attribute_class_filter.clear();
		text = &attribute_class_filter;
		RegisterAtTagExit(&ConfigReader::ClearTextPointers);
	}
	void VisitAttributeClassName(cpcl::XmlReader*) {
		attribute_class_name.clear();
		wtext = &attribute_class_name;
		RegisterAtTagExit(&ConfigReader::ClearTextPointers);
	}

	void PushStack(std::string const &tag, cpcl::XmlReader *reader) {
		if (must_call.size() != (size_t)reader->Depth())
			throw config_error("ConfigReader()::PushTag(): invalid xml");

		MustCall v;
		if (!must_call.empty())
			v.path = must_call.back().path;
		v.path += tag;
		must_call.push_back(v);

		InvokeHandlers(v.path, reader);
	}
	void PopStack(cpcl::XmlReader *reader) {
		DUMBASS_CHECK(!must_call.empty());

		must_call.back().Release(this, reader);
		must_call.pop_back();

		if (must_call.size() != (size_t)reader->Depth())
			throw config_error("ConfigReader()::PopTag(): invalid xml");
	}

	/*void ReadConfigTag(cpcl::XmlReader *reader) {
		DUMBASS_CHECK(wtext);

		*wtext = cpcl::WStringPiece(*wtext).trim(cpcl::WStringPieceFromLiteral(L" \t\r\n\\")).as_string();
		if (wtext->empty())
			throw config_error("ConfigReader()::Read(): <config> can't be empty");
		wtext->insert(0, 1, L'\\');
		wtext->insert(0, params_path.data(), params_path.size());
		wtext = 0;
	}*/

	std::string *text;
	std::wstring *wtext;
	cpcl::ScopedBuf<wchar_t, 0x100> conv_buf;
	void ClearTextPointers(cpcl::XmlReader*) {
		text = 0;
		wtext = 0;
	}
	void ReadText(cpcl::XmlReader *reader) {
		if (!((wtext) || (text)))
			return;
		cpcl::StringPiece s = reader->Value_UTF8();
		if (s.empty())
			return;

		if (wtext) {
			conv_buf.Alloc(s.size());
			int const conv_result = cpcl::TryConvertUTF8_UTF16(s, conv_buf.Data(), conv_buf.Size());
			if (conv_result < 0)
				throw config_error("ConfigReader()::ReadText({0:tag_path}): TryConvertUTF8_UTF16 fails"); // must_call.back().tag_path.c_str()
			
			wtext->append(conv_buf.Data(), (size_t)conv_result);
		} else if (text) {
			text->append(s.data(), s.size());
		}
	}

	void Read(cpcl::XmlReader *reader) {
		while (reader->Read()) {
			// printf("%d : <%s>\n", reader->Depth(), reader->Name_UTF8().as_string().c_str());

			std::string tag = reader->Name_UTF8().as_string();
			ToASCIIlower(const_cast<char*>(tag.c_str()));
			if (cpcl::StringPiece(tag) == cpcl::StringPieceFromLiteral("#text"))
				ReadText(reader);
			else if (cpcl::StringPiece(tag) == cpcl::StringPieceFromLiteral("xml"))
				continue;
			else if (reader->IsEndElement())
				PopStack(reader);
			else
				PushStack(tag, reader);
		}
	}
};

namespace cpcl {

template<class Char>
inline BasicStringPiece<Char, std::char_traits<Char> > DirName(BasicStringPiece<Char, std::char_traits<Char> > const &s) {
	if (s.empty())
		return s;

	Char const *path_head = s.data() - 1;
	Char const *i = path_head + s.size();
	bool t = false;
	for (; i != path_head; --i) {
		if (*i == (Char)'\\')
			t = true;
		else if (t)
			break;
	}
	return (t) ? BasicStringPiece<Char, std::char_traits<Char> >(s.data(), i - path_head) : s;
}

} // namespace cpcl

/* cpcl::LibXmlInit(); cpcl::LibXmlCleanup(); -> DllMain() */
std::list<DetectorInfo> ReadClassifierConfig(cpcl::WStringPiece const &path) {
	std::list<DetectorInfo> r;
	try {
		cpcl::FileStream *in_;
		if (cpcl::FileStream::Read(path, &in_)) {
			std::auto_ptr<cpcl::FileStream> in(in_);

			cpcl::XmlReader *reader_;
			if (cpcl::XmlReader::Create(in.get(), &reader_)) {
				std::auto_ptr<cpcl::XmlReader> reader(reader_);

				ConfigReader config_reader(DirName(path));
				config_reader.Read(reader.get());
				
				r.swap(config_reader.detectors);
			}
		}
	} catch (std::exception const &e) {
		cpcl::Error(e.what());
	}
	return r;
}

#if 0

static void __stdcall AssertHandler(char const *s, char const *file, unsigned int line) {
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "%s at %s:%d epic fail", s, file, line);
	
	throw std::runtime_error(s);
}

void TestReadConfig() {
	cpcl::SetAssertHandler(AssertHandler);

	std::list<DetectorInfo> r = ReadConfig(cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\cf_params.xml"));
	
	assert(r.size() == 1);

	GUID id = { 0xA34CDBD5, 0x0EF2, 0x45FB, { 0x89, 0x0A, 0xCE, 0xA8, 0x7E, 0x8F, 0x44, 0x59 } };
	assert(r.front().id == id);
	assert(cpcl::WStringPiece(r.front().name) == cpcl::WStringPieceFromLiteral(L"FaceDetector"));
	assert(cpcl::WStringPiece(r.front().config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\engine1.xml"));

	assert(r.back().attributes.size() == 2);

	AttributeInfo &age_attribute = r.back().attributes.front();
	GUID age_attribute_id = { 0x81F4A8EE, 0x2163, 0x11E2, { 0xA3, 0xAA, 0x00, 0x1D, 0xBA, 0x24, 0xCA, 0xE9 } };
	assert(age_attribute.id == age_attribute_id);
	assert(cpcl::WStringPiece(age_attribute.name) == cpcl::WStringPieceFromLiteral(L"Age attribute"));
	assert(cpcl::WStringPiece(age_attribute.config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\adult_child.xml"));

	assert(age_attribute.classes.size() == 2);

	std::map<int, std::wstring>::const_iterator i = age_attribute.classes.find(0);
	assert(i != age_attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"adult"));
	i = age_attribute.classes.find(1);
	assert(i != age_attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"child"));

	AttributeInfo &gender_attribute = r.back().attributes.back();
	GUID gender_attribute_id = { 0xE4FBC3C0, 0x2163, 0x11E2, { 0xA3, 0xE8, 0x00, 0x1D, 0xBA, 0x24, 0xCA, 0xE9 } };
	assert(gender_attribute.id == gender_attribute_id);
	assert(cpcl::WStringPiece(gender_attribute.name) == cpcl::WStringPieceFromLiteral(L"Gender attribute"));
	assert(cpcl::WStringPiece(gender_attribute.config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\male_female.xml"));

	assert(gender_attribute.classes.size() == 2);
	
	i = gender_attribute.classes.find(0);
	assert(i != gender_attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"female"));
	i = gender_attribute.classes.find(1);
	assert(i != gender_attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"male"));
}

void TestReadConfig_() {
	cpcl::SetAssertHandler(AssertHandler);

	std::list<DetectorInfo> r;
	try {
		cpcl::LibXmlInit();
		
		cpcl::WStringPiece path = cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\config.xml");
		char const s[] = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\
<detectors>\
	<detector>\
		<id>A34CDBD5-0EF2-45fb-890A-CEA87E8F4459</id>\
		<name>FaceDetector</name>\
		<config>engine1.xml</config>\
		<!--<tag/>-->\
	</detector>\
	<detector>\
		<id>B34CDBD5-0EF2-45fb-890A-CEA87E8F4459</id>\
		<name>PolygonDetector</name>\
		<config>polygon1.xml</config>\
		<attributes>\
			<attribute>\
				<id>C34CDBD5-0EF2-45fb-890A-CEA87E8F4459</id>\
				<name>PolygonAttribute</name>\
				<config>polygon_attribute1.xml</config>\
				<classes>\
					<class>\
						<filter> -1 </filter>\
						<name>class name -1</name>\
					</class>\
					<class>\
						<filter>1</filter>\
						<name>class name 1</name>\
					</class>\
				</classes>\
			</attribute>\
		</attributes>\
	</detector>\
</detectors>\
";
		cpcl::MemoryStream in((unsigned char*)s, arraysize(s) - 1);

		cpcl::XmlReader *reader_;
		if (cpcl::XmlReader::Create(&in, &reader_)) {
			std::auto_ptr<cpcl::XmlReader> reader(reader_);
			
			ConfigReader config_reader(DirName(path));
			config_reader.Read(reader.get());

			r.swap(config_reader.detectors);
		}

		cpcl::LibXmlCleanup();
	} catch (std::exception const &e) {
		cpcl::Error(e.what());
	}

	assert(r.size() == 2);

	GUID id = { 0xA34CDBD5, 0x0EF2, 0x45FB, { 0x89, 0x0A, 0xCE, 0xA8, 0x7E, 0x8F, 0x44, 0x59 } };
	assert(r.front().id == id);
	assert(cpcl::WStringPiece(r.front().name) == cpcl::WStringPieceFromLiteral(L"FaceDetector"));
	assert(cpcl::WStringPiece(r.front().config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\engine1.xml"));
	assert(r.front().attributes.empty());

	GUID id_ = { 0xB34CDBD5, 0x0EF2, 0x45FB, { 0x89, 0x0A, 0xCE, 0xA8, 0x7E, 0x8F, 0x44, 0x59 } };
	assert(r.back().id == id_);
	assert(cpcl::WStringPiece(r.back().name) == cpcl::WStringPieceFromLiteral(L"PolygonDetector"));
	assert(cpcl::WStringPiece(r.back().config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\polygon1.xml"));

	assert(r.back().attributes.size() == 1);

	AttributeInfo &attribute = r.back().attributes.front();
	GUID id__ = { 0xC34CDBD5, 0x0EF2, 0x45FB, { 0x89, 0x0A, 0xCE, 0xA8, 0x7E, 0x8F, 0x44, 0x59 } };
	assert(attribute.id == id__);
	assert(cpcl::WStringPiece(attribute.name) == cpcl::WStringPieceFromLiteral(L"PolygonAttribute"));
	assert(cpcl::WStringPiece(attribute.config) == cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\draw\\bin\\Debug\\cf_params\\polygon_attribute1.xml"));

	assert(attribute.classes.size() == 2);

	std::map<int, std::wstring>::const_iterator i = attribute.classes.find(-1);
	assert(i != attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"class name -1"));
	i = attribute.classes.find(1);
	assert(i != attribute.classes.end());
	assert(cpcl::WStringPiece((*i).second) == cpcl::WStringPieceFromLiteral(L"class name 1"));
}

#endif
