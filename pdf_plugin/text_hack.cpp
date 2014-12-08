// Copyright (C) 2012 Yuri Agafonov
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

#include <math.h>

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

#include "text_hack.h"

#include <poppler/GlobalParams.h>
//#include <poppler/Page.h>
#include <poppler/TextOutputDev.h>
#include <poppler/GfxState.h>

#include <string_util.h>
#include <trace.h>

template<typename T> 
bool is_equal_eps(T const &a, T const &b) { return (fabs(a - b) < (T)1e-9); }

//#define INCH_MM  (25.4)
//#define POINT2MM (INCH_MM/72.0)
//#define MM2POINT (72.0/INCH_MM)

//int cp1251_to_uc(const char c);

class TextWordEx : public TextWord {
public:
	class inject_TextWord : public TextWord {
	public:
		int inject_rot() { return this->rot; }
		int inject_charPos() { return this->charPos; }
		TextFontInfo* inject_font() { return this->font; }
		double inject_fontSize() { return this->fontSize; }

		static inject_TextWord* ctor(TextWord *word) { return (inject_TextWord*)word; }
	};

	RectContainer::Rects rects;

	/*override*/void addChar(GfxState *state, double x, double y,
		double dx, double dy, CharCode c, Unicode u) {
		TextWord::addChar(state, x, y, dx, dy, c, u);

		// double cw = (std::max)(dx,dy);
    double fs = state->getTransformedFontSize();
    double a  = fs*state->getFont()->getAscent();  // >0
    double d  = fs*state->getFont()->getDescent(); // <0

		double xMin, xMax, yMin, yMax;
		switch (rot) {
			case 0:
				xMin = x; xMax = x + dx;
				yMin = y - a; yMax = y - 0.5*d;
				if (is_equal_eps(yMin, yMax)) {
					yMin = y; yMax = y + 1;
				}
				break;
			case 1:
				xMin = x + 0.5*d; xMax = x + a;
				if (is_equal_eps(xMin, xMax)) {
					xMin = x; xMax = x + 1;
				}
				yMin = y; yMax = y + dy;
				break;
			case 2:
				xMin = x + dx; xMax = x;
				yMin = y + 0.5*d; yMax = y + a;
				if (is_equal_eps(yMin, yMax)) {
					yMin = y; yMax = y + 1;
				}
				break;
			case 3:
				xMin = x - a; xMax = x - 0.5*d;
				if (is_equal_eps(xMin, xMax)) {
					xMin = x; xMax = x + 1;
				}
				yMin = y + dy; yMax = y;
				break;
		}
		setRect(xMin, yMin, xMax, yMax);
	}
	/*override*/void merge(TextWord *word) {
		//Merge <word> onto the end of <this>
		TextWord::merge(word);

		std::copy(((TextWordEx*)word)->rects.begin(), ((TextWordEx*)word)->rects.end(), std::inserter(rects, rects.end()));
	}
	void setRect(double x1, double y1, double x2, double y2) {
		//rects.push_back(CaptureRect(double(x1*POINT2MM), double(y1*POINT2MM), double(x2*POINT2MM), double(y2*POINT2MM)));

		// in djvu_page rect.y1 = z_->rect.ymin && rect.y2 = z_->rect.ymax, so
		rects.push_back(RectContainer::Rect((int)x1, (int)y1, (int)x2, (int)y2));
	}

	TextWordEx(inject_TextWord *word, GfxState *state, double x0, double y0) 
		: TextWord(state, word->inject_rot(), x0, y0, word->inject_charPos(), word->inject_font(), word->inject_fontSize()) {}
};

// Maximum distance between baselines of two words on the same line,
// e.g., distance between subscript or superscript and the primary
// baseline, as a fraction of the font size.
#define maxIntraLineDelta 0.5

// Minimum inter-word spacing, as a fraction of the font size.  (Only
// used for raw ordering.)
#define minWordSpacing 0.1

class TextPageEx : public TextPage {
public:
	class inject_TextPage : public TextPage {
	public:
		GBool inject_rawOrder() { return this->rawOrder; }

		static inject_TextPage* ctor(TextPage *page) { return (inject_TextPage*)page; }
	};

	/*override*/void beginWord(GfxState *state, double x0, double y0) {
		TextPage::beginWord(state, x0, y0); // curWord = new TextWord(state, rot, x0, y0, charPos, curFont, curFontSize);
		if (curWord != (TextWord*)NULL) {
			TextWord *oldCurWorld = curWord;
			curWord = new TextWordEx(TextWordEx::inject_TextWord::ctor(curWord), state, x0, y0); 
			delete oldCurWorld;
		}
	}

	TextPageEx(inject_TextPage *page) : TextPage(page->inject_rawOrder()) {}

	void dumpRaw(RectContainer &rectContainer) {
		/*UnicodeMap *uMap = globalParams->getTextEncoding();
		Unicode u;*/

		for (TextWord *word = rawWords; word; word = word->next) {
			int n = word->getLength();
			for(int j = 0; j < n; ++j) {
				rectContainer.SetRect(((TextWordEx*)word)->rects[j]);

				/*char buf[0x10] = { 0 };
				int tLen = uMap->mapUnicode(word->text[j], &buf[0], sizeof(buf));*/
				if (word->text[j] >> 8) {
					//0x96 U+2013 EN DASH
					if (0x96 == word->text[j])
						rectContainer.text.append(1, (wchar_t)0x2013);
					else
						rectContainer.text.append(1, word->text[j]);
				} else {
					rectContainer.text.append(1, (wchar_t)cpcl::CP1251_UTF16((unsigned char)word->text[j]));
				}
			}

			if (word->next &&
				fabs(word->next->base - word->base) < (maxIntraLineDelta * word->fontSize)) {
				if (word->next->xMin > (word->xMax + (minWordSpacing * word->fontSize))) {
					rectContainer.text.append(1, (wchar_t)32);
					//u=c=32; rectContainer.drawChar(u,&c,1);
				}
			} else {
				rectContainer.text.append(1, (wchar_t)13);
				rectContainer.text.append(1, (wchar_t)10);
				//u=c=13; rectContainer.drawChar(u,&c,1);
				//u=c=10; rectContainer.drawChar(u,&c,1);
			}
		}

		rectContainer.text.append(1, (wchar_t)13);
		rectContainer.text.append(1, (wchar_t)10);
		//u=c=13; rectContainer.drawChar(u,&c,1);
		//u=c=10; rectContainer.drawChar(u,&c,1);

		/*uMap->decRefCnt();*/
	}
	void dumpLayout(RectContainer &rectContainer) {
		/*UnicodeMap *uMap = globalParams->getTextEncoding();
		char c; Unicode u;*/
		for (TextFlow *flow = flows; flow; flow = flow->next) {
			for (TextBlock *blk = flow->blocks; blk; blk = blk->next) {
				//bool startNewLine(true), startNewWord(true);
				for (TextLine *line = blk->lines; line; line = line->next) {
					TextWord *word_prev(0);
					for (TextWord *word = line->words; word; word = word->next) {
						int n = word->getLength();
						std::wstring wordText(word->text, word->text + word->getLength());

						if (word_prev) {
							if (word->rot & 1) {
								if (word->yMax < (word_prev->yMin - (minWordSpacing * word_prev->fontSize))) {
									rectContainer.text.append(1, (wchar_t)32);
									//u=c=32; rectContainer.drawChar(u,&c,1);
								}
							} else {
								if (word->xMin > (word_prev->xMax + (minWordSpacing * word_prev->fontSize))) {
									rectContainer.text.append(1, (wchar_t)32);
									//u=c=32; rectContainer.drawChar(u,&c,1);
								}
							}
						}
						//if (word_prev &&
						//	(word->xMin > (word_prev->xMax + (minWordSpacing * word_prev->fontSize)))) {
						//	u=c=32; rectContainer.drawChar(u,&c,1);
						//}
						word_prev = word;
						for(int j = 0; j < n; ++j) {
							rectContainer.SetRect(((TextWordEx*)word)->rects[j]);

							/*char buf[0x10] = { 0 };
							int tLen = uMap->mapUnicode(word->text[j], &buf[0], sizeof(buf));*/
							if (word->text[j] >> 8) {
								//0x96 U+2013 EN DASH
								if (0x96 == word->text[j])
									rectContainer.text.append(1, (wchar_t)0x2013);
								else
									rectContainer.text.append(1, word->text[j]);
							} else {
								rectContainer.text.append(1, (wchar_t)cpcl::CP1251_UTF16((unsigned char)word->text[j]));
							}
							//if (tLen > 1) { // why > and not >= ??
							//	// #$!*%!!!@#$!%#!$%#
							//	//0x96 U+2013 EN DASH
							//	if (0x96 == word->text[j]) rectContainer.drawChar(wchar_t(0x2013), buf, );
							//	else layout.drawChar(word->text[j], symbolRect);
							//} else {
							//	layout.drawChar(cp1251_to_uc(buf[0]), symbolRect);
							//}
						}

						//if (word->next) {
						//	u=c=32; rectContainer.drawChar(u,&c,1);
						//}
					}

					rectContainer.text.append(1, (wchar_t)13);
					rectContainer.text.append(1, (wchar_t)10);
					//u=c=13; rectContainer.drawChar(u,&c,1);
					//u=c=10; rectContainer.drawChar(u,&c,1);
					//if (((TextLine*)0 == line->next) && (blk->next)) {
					//		//~ this is a bit of a kludge - we should really do a more
					//		//~ intelligent determination of paragraphs
					//		if (!is_equal_eps(blk->next->lines->words->fontSize, blk->lines->words->fontSize)) {
					//			u=c=13; rectContainer.drawChar(u,&c,1);
					//			u=c=10; rectContainer.drawChar(u,&c,1);
					//		}
					//}
					//if (line->next) {
					//	startNewLine = false;
					//	} else if (blk->next) {
					//		//~ this is a bit of a kludge - we should really do a more
					//		//~ intelligent determination of paragraphs
					//		if (is_equal_eps(blk->next->lines->words->fontSize, blk->lines->words->fontSize) {
					//			startNewLine = false;
					//		} else {
					//			startNewLine = true; layout.endLine(o);
					//		}
					//}
				}		
			}
		}
		rectContainer.text.append(1, (wchar_t)13);
		rectContainer.text.append(1, (wchar_t)10);
		//u=c=13; rectContainer.drawChar(u,&c,1);
		//u=c=10; rectContainer.drawChar(u,&c,1);

		/*uMap->decRefCnt();*/
	}
};

//void OnTextOutputFunc(void *stream, char *text, int len) { ((std::string*)stream)->append(text, len); }
//static void onTextOutputFunction(void *g,char*,int len) { *((int *)g) |= len; }

//hackRectContainer* retreiveLayout(PDFDoc &pdfDoc, int pageNum) {
//	hackRectContainer *rr = new hackRectContainer();
//	hackRectContainer &r = *rr;
//
//	std::string outText;
//	//TextOutputDev dev(OnTextOutputFunc, &outText, gTrue, gFalse);
//	TextOutputDev dev(OnTextOutputFunc, &outText, gFalse, gTrue);
//	if (dev.text) { TextPage* oldText = dev.text; dev.text = new TextPageEx(TextPageEx::inject_TextPage::ctor(oldText)); delete oldText; }
//	/*****/
//	pdfDoc.displayPage(&dev, pageNum, 72, 72, 0, gFalse, gFalse);
//	if (!outText.empty()) {
//		TextPageEx *textPage = (TextPageEx*)dev.text;
//		//textPage->dumpLayout(r);
//		textPage->dumpRaw(r);
//	}
//
//	return rr;
//}

static wchar_t const hyphen_characters[] = { 0x002D, 0x00AD, 0x2010, 0x2012, 0x2013, 0x2014, 0x2212 };
inline bool is_hyphen(wchar_t c) {
	return wmemchr(hyphen_characters, c, arraysize(hyphen_characters)) != NULL;
}

// wchar_t const trim_characters[] = { L'.', L',', L':', L';', L'<', L'>', L'«', L'»', L'(', L')', L'?', L'|', L'/', L'\\', L'-', L'\x20', 0 };
static wchar_t const *trim_characters = L"\x22\x27.,:;<>«»()?|/\\-\x20\x09\x0A\x0D";
inline std::wstring wstr_trim(std::wstring const &v) {
	if (v.empty())
		return std::wstring(); 

	std::wstring r; r.reserve(v.size());
	int head, tail;
	for (head = 0; head < (int)v.size(); ++head) {
		if (!wcschr(trim_characters, v[head]))
			break;
	}
	for (tail = ((int)v.size()) - 1; tail > head; --tail) {
		if (!wcschr(trim_characters, v[tail]))
			break;
	}

	if (tail > head) {
		r = v.substr(head, tail - head + 1);
	}

	std::wstring rr; rr.reserve(r.size());
	for(size_t i = 0; i < r.size(); ++i) {
		//if ('\x20' != r[i]) {
			if (is_hyphen(r[i]))
				rr += L'-';
			else
				rr += r[i];
		//}
	}

	return rr;
}

inline bool wstr_interestingWord(std::wstring const &v) {
	if (v.size() > 3) {
		struct symbolRange { wchar_t head, tail; };

		symbolRange ranges[] = { { L'a', L'z' }, /*{ L'0', L'9' },*/ { L'A', L'Z' }, { 0x400, 0x4FF } }; 
		// must contain at least two symbols from ranges
		size_t symbolsCount(0);
		for (size_t i = 0; i < v.size(); ++i) {
		for (size_t j = 0; j < (sizeof(ranges) / sizeof(*ranges)); ++j) {
			if ((v[i] >= ranges[j].head) && (v[i] <= ranges[j].tail)) {
				if (++symbolsCount > 1)
					return true;
			}
		}
		}
	}
	return false;
}

inline std::wstring& wstr_lower(std::wstring &v) {
  static int cp1251_lower[32] = {
    0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,
    0x043F,0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,
    0x044E,0x044F /*??? why letter 0x044F missed ???*/
  };
  static int ascii_lower[26] = {
    0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
    0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A /*??? why letter 0x007A missed ???*/
  };

	for (size_t i = 0; i < v.size(); ++i) {
		wchar_t &c = v[i];
    if ((c >= 0x0410) && (c <= 0x042F)) {
      c = (wchar_t)cp1251_lower[c - 0x0410];
    } else if ((c >= 0x0041) && (c <= 0x005A)) {
      c = (wchar_t)ascii_lower[c - 0x0041];
    }
	}
	return v;
}

template<class _ArgumentType, class _ResultType, class _Fn>
struct __unaryFunctionAdapter : public std::unary_function<_ArgumentType, _ResultType> {
  _Fn function_;

  __unaryFunctionAdapter(_Fn v) : function_(v) {}
  _ResultType operator()(_ArgumentType const &v) const { return function_(v); }
};
template<class _ArgumentType, class _ResultType, class _Fn>
__unaryFunctionAdapter<_ArgumentType, _ResultType, _Fn> __unaryFunction(_Fn v) {
  return __unaryFunctionAdapter<_ArgumentType, _ResultType, _Fn>(v);
}

void addParamsWords(std::vector<std::wstring> &v, bool is_error_words) {
	std::transform(v.begin(), v.end(), v.begin(), wstr_trim);
  v.erase(std::remove_if(v.begin(), v.end(), std::not1(__unaryFunction<std::wstring, bool>(wstr_interestingWord))), v.end());
  std::for_each(v.begin(), v.end(), wstr_lower);
	if (is_error_words)
		globalParams->setErrorWords(v);
	else
		globalParams->setHyphenWords(v);
}

#include <sstream>
#include "text_builder.hpp"

template<class _PredicateType>
struct __unaryBoolNot {
	_PredicateType predicate_;

	__unaryBoolNot(_PredicateType const &predicate) : predicate_(predicate) {}
	bool operator()(typename _PredicateType::value_type const v) { return !(predicate_(v)); }
};
template<class _PredicateType>
inline __unaryBoolNot<_PredicateType> __unaryNot(_PredicateType const v) { return __unaryBoolNot<_PredicateType>(v); }
template<class _NodeType>
inline double median(_NodeType const v) { return (std::min)(v->xMin, v->xMax) + (fabs(v->xMax - v->xMin) / 2); }
template<class _NodeType>
inline double width(_NodeType const v) { return fabs(v->xMax - v->xMin); }
template<class _NodeType>
struct absurdeComparator {
	typedef _NodeType value_type;

	_NodeType const v_;
	double v_median_, v_width_;

	absurdeComparator(_NodeType const v) : v_(v) { v_median_ = median(v_); v_width_ = fabs(v->xMax - v->xMin); }
	bool operator()(_NodeType const v) {
		double v_median = median(v);
		double smallest_width = (std::min)(v_width_, fabs(v->xMax - v->xMin));
		return fabs(v_median - v_median_) < (0.2 * smallest_width);
	}
};
template<class _NodeType>
inline absurdeComparator<_NodeType> makeAbsurdComparator(_NodeType const v) { return absurdeComparator<_NodeType>(v); }

struct eliglibleCluster {
	typedef std::vector<TextFlow*> cluster_t;
	typedef cluster_t::iterator cluster_i;
	
	cluster_t &v_;
	// std::vector<double> v_medians_;

	eliglibleCluster(cluster_t &v) : v_(v) {} // std::transform(v_.begin(), v_.end(), std::back_inserter(v_medians_), median<cluster_t::value_type>); }
	eliglibleCluster(eliglibleCluster const &v) : v_(v.v_) {}
	bool eliglible_x(cluster_t const &v, cluster_t::value_type const &top, cluster_t::value_type const &down) const {
		std::vector<double> v_medians; std::transform(v.begin(), v.end(), std::back_inserter(v_medians), median<cluster_t::value_type>);

		double top_median(median(top)), down_median(median(down));
		for (size_t i = 0; i < v_medians.size(); ++i) {
			if ((fabs(v_medians[i] - top_median) < ((width(top) / 2) + (width(v[i]) / 2)))
				|| (fabs(v_medians[i] - down_median) < ((width(down) / 2) + (width(v[i]) / 2)))) {
				return true;
			}
		}
		return false;
	}
	bool operator()(cluster_t const &v) {
		if (&v == &v_) {
			return false;
		}

		for (cluster_t::difference_type i = 0; i < ((cluster_t::difference_type)(v_.size() - 1)); ++i) {
			if ((v_[i]->yMax <= v.front()->yMin)
				&& (v_[i+1]->yMin >= v.back()->yMax)) {
					if (eliglible_x(v, v_[i], v_[i+1])) {
						cluster_i _where = v_.begin(); std::advance(_where, i+1);
						copy(v.begin(), v.end(), std::inserter(v_, _where)); // cluster_main.insert(where, cluster_i);
						return true;
					}
					break;
			}
		}
		return false;
	}
};
inline std::vector<std::vector<TextFlow*>>& kenny(std::vector<std::vector<TextFlow*>> &clusters) {
	typedef std::vector<std::vector<TextFlow*>> clusters_t;
	typedef clusters_t::iterator clusters_i;

	clusters_i main = clusters.begin();
	while(main != clusters.end()) {
		clusters_i i = find_if(clusters.begin(), clusters.end(), eliglibleCluster(*main));
		if (i != clusters.end()) {
			clusters.erase(i);
			main = clusters.begin(); // aarrrrgggghhhhhhhxxxxx!!!!!!!!!!!!!!!!!
		} else {
			++main;
		}
	}

	return clusters;
}

namespace cpcl {

inline void Info(StringPiece const &s) {
	if (TRACE_LEVEL & CPCL_TRACE_LEVEL_INFO)
		TraceString(CPCL_TRACE_LEVEL_DEBUG, s);
}

}

static void onTextOutputFunction(void *g, char*, int len) { *((int *)g) |= len; }
bool retreiveLayout(PDFDoc *pdfDoc, int pageNum, RectContainer *r_) {
	if (!pdfDoc || !r_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("retreive layout fails - invalid args"));
		return false;
	}
	RectContainer &r = *r_;

  try {
	int hasText(0);
	TextOutputDev dev(onTextOutputFunction, &hasText, gTrue, gFalse);
	cpcl::Info(cpcl::StringPieceFromLiteral("retreive text"));
	if (dev.text) { 
		TextPage* oldText = dev.text; dev.text = new TextPageEx(TextPageEx::inject_TextPage::ctor(oldText)); delete oldText;
		dev.actualText = new ActualText(dev.text);
	} else {
		cpcl::Error(cpcl::StringPieceFromLiteral("retreive layout fails - absent TextOutputDev"));
		return false;
	}
	/*****/
	cpcl::Info(cpcl::StringPieceFromLiteral("before dispaly page"));
  pdfDoc->displayPages(&dev, pageNum, pageNum, 72, 72, 0, gFalse, gFalse, gFalse);
  cpcl::Info(cpcl::StringPieceFromLiteral("after dispaly page"));
	if (0 == hasText) {
		cpcl::Info(cpcl::StringPieceFromLiteral("hasn't text"));
		return false;
	}
	
	TextPageEx *textPage = (TextPageEx*)dev.text;
	std::vector<TextFlow*> flows;
	for (TextFlow *flow = textPage->flows; flow; flow = flow->next) {
		flows.push_back(flow);
	}
	
	std::vector<std::vector<TextFlow*>> flows_clusters;
	while (!flows.empty()) {
		std::vector<TextFlow*> column_flows; column_flows.push_back(flows.front()); // flows.pop_front();
		std::remove_copy_if(++flows.begin(), flows.end(), std::back_inserter(column_flows), __unaryNot(makeAbsurdComparator(column_flows.front())));
		
		std::vector<TextFlow*> __flows;
		std::remove_copy_if(++flows.begin(), flows.end(), std::back_inserter(__flows), makeAbsurdComparator(column_flows.front()));
		flows = __flows;
		
		if (!column_flows.empty())
			flows_clusters.push_back(column_flows);
	}
	
	kenny(flows_clusters);
	{
		text2text::textBuilder builder(r/*, globalParams->hypens*/);
		builder.addText(flows_clusters);
	}
	} catch(std::exception const &e) {
		char const *s = e.what();
		if (s && *s) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"retreive layout fails - exception: %s",
				s);
		} else {
			cpcl::Error(cpcl::StringPieceFromLiteral("retreive layout fails - get std::exception"));
		}
		return false;
  } catch(...) {
    cpcl::Error(cpcl::StringPieceFromLiteral("retreive layout fails - get unknown exception"));
		return false;
  }

	return true;
}
