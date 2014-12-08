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

#include <vector>
#include <string>

#include <com_ptr.hpp>
#include <istream_wrapper.h>

#include <dumbassert.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <internal_com.h>

#include "variantItemStorage.h"

#include "customclassfactory_list.h"
#include "istream_util.hpp"

//#include "istreamWrapper.hpp"

//WriteClassStm, ReadClassStm - write, read CLSID

#define COMPILE_TIME_ASSERT(expr) // ...

template<class T>
inline HRESULT streamRead(cpcl::IOStream &input, T *v) {
	return (input.Read(v, sizeof(T)) == sizeof(T)) ? S_OK : STG_E_READFAULT;
}
template<class T>
inline HRESULT streamWrite(cpcl::IOStream &output, T const &v) {
	return (output.Write(&v, sizeof(T)) == sizeof(T)) ? S_OK : STG_E_READFAULT;
}

struct binaryVariantBuffer {
  binaryVariantBuffer() : dynamicBuffer(NULL), binaryBufferSize(0), dynamicBufferCapacity(0) {}
  ~binaryVariantBuffer() {
		clear();
		freeDynamicBuffer();
	}

	void clear() {
		vt = VT_EMPTY; persistStream = (IPersistStream*)0;
		binaryBufferSize = 0;
	}
  void freeDynamicBuffer() {
    if (dynamicBuffer) {
      delete [] dynamicBuffer;
      dynamicBuffer = NULL;
    }
    dynamicBufferCapacity = binaryBufferSize = 0;
  }
  size_t size() const { return binaryBufferSize; }

	//template<class _StreamType, class T>
	//inline HRESULT streamRead(_StreamType &stream, T *v) {
	//	return (stream.read(v, sizeof(T)) == sizeof(T)) ? S_OK : STG_E_READFAULT;
	//}
  HRESULT load(IStream *v) {
		clear();

		if (!v)
			return E_POINTER;
		cpcl::IStreamWrapper stream(v);

		RINOK(streamRead(stream, &vt))
		if (vt != VT_EMPTY) {
			if (VT_UNKNOWN == vt) {
				CLSID classID;
				RINOK(cpcl::read_pod(v, &classID))
				ScopedComPtr<ICustomClassFactory> factory;
				RINOK(findFactory(classID, &factory))
				RINOK(factory->CreateInstance(classID, &persistStream))
				RINOK(persistStream->Load(v));
			} else {
				size_t binaryBufferSize_;
				RINOK(streamRead(stream, &binaryBufferSize_))
				if (binaryBufferSize_ > 0) {
					unsigned char *data = binaryBuffer(binaryBufferSize_);
					if (!data)
						return E_OUTOFMEMORY;

					if (stream.Read(data, binaryBufferSize_) != binaryBufferSize_)
						return STG_E_READFAULT;
				}
			}
		}
		return S_OK;
  }
	/*
	VARTYPE vt;
	size_t size;
	[data]
	*/
  HRESULT save(IStream *v) {
    //if (VT_EMPTY == vt)
    //  return S_FALSE;
		if (!v)
			return E_POINTER;
		cpcl::IStreamWrapper stream(v);

		RINOK(streamWrite(stream, vt))
		if (vt != VT_EMPTY) {
			if (VT_UNKNOWN == vt) {
				if (!persistStream)
					return E_UNEXPECTED;
				CLSID classID;
				RINOK(persistStream->GetClassID(&classID))
				RINOK(cpcl::write_pod(v, classID))
				RINOK(persistStream->Save(v, TRUE))
			} else {
				RINOK(streamWrite(stream, size()))
				if (size() > 0) {
					if (stream.Write(binaryBuffer(), size()) != size())
						return STG_E_MEDIUMFULL;
				}
			}
		}
		return S_OK;
  }

  HRESULT attach(comUtility::PropVariantRenamed const &v) {
    clear();
    
    if (v.vt == VT_ARRAY) {
      SAFEARRAY *parray = v.parray;
      if (!parray)
        return E_INVALIDARG;

      if (::SafeArrayGetDim(parray) != 1)
        return E_INVALIDARG;
      LONG lowerBound, upperBound;
      RINOK(::SafeArrayGetLBound(parray, 1, &lowerBound))
      RINOK(::SafeArrayGetUBound(parray, 1, &upperBound))
      size_t arraySize = static_cast<size_t>(upperBound - lowerBound + 1);

      unsigned char *binaryBuffer_ = binaryBuffer(arraySize);
      if (!binaryBuffer_)
        return E_OUTOFMEMORY;
      
      unsigned char HUGEP *arrayData;
      RINOK(::SafeArrayAccessData(parray, (void HUGEP**)&arrayData))
      memcpy(binaryBuffer_, arrayData, arraySize);
      RINOK(::SafeArrayUnaccessData(parray))

      vt = VT_ARRAY;
    } else if (v.vt == VT_UNKNOWN) {
      if ((!v.punkVal) || FAILED(v.punkVal->QueryInterface(IID_IPersistStream, (void**)&persistStream)))
        return E_INVALIDARG;

      vt = VT_UNKNOWN;
    } else if (v.vt == VT_BSTR) {
      if (!v.bstrVal)
        return E_INVALIDARG;

      size_t len = ::SysStringByteLen(v.bstrVal);
			if (0 == len) {
				binaryBufferSize = 0;
			} else {
				unsigned char *binaryBuffer_ = binaryBuffer(len);
				if (!binaryBuffer_)
					return E_OUTOFMEMORY;
				memcpy(binaryBuffer_, v.bstrVal, len);
			}

      vt = VT_BSTR;
    } else {
      RINOK(attachPODType(v))
    }

    return S_OK;
  }
  HRESULT attach(unsigned char const *buffer, size_t bufferSize) {
    clear();

    unsigned char *binaryBuffer_ = binaryBuffer(bufferSize); // binaryBufferSize != bufferSize ???
    if (!binaryBuffer_)
      return E_INVALIDARG;

    memcpy(binaryBuffer_, buffer, bufferSize);
    vt = VT_ARRAY;
    
    return S_OK;
  }
  HRESULT detach(comUtility::PropVariantRenamed *v) {
    if (!v)
      return E_POINTER;
    if (VT_EMPTY == vt)
      return v->Clear();

    RINOK(v->Clear())

    if (vt == VT_ARRAY) {
      SAFEARRAY *parray = ::SafeArrayCreateVector(VT_UI1, 0, size());
      if (!parray)
        return E_FAIL;
      
      unsigned char HUGEP *arrayData;
      RINOK(::SafeArrayAccessData(parray, (void HUGEP**)&arrayData))
      memcpy(arrayData, binaryBuffer(), size());
      RINOK(::SafeArrayUnaccessData(parray))

      v->vt = VT_ARRAY; v->parray = parray;
    } else if (vt == VT_UNKNOWN) {
      if (!persistStream)
        return E_UNEXPECTED;

      // ::VariantClear for VT_UNKNOWN call IUnknown::Release, i.e. carry ownership
      ScopedComPtr<IUnknown> transferOwnership(static_cast<IUnknown*>((IPersistStream*)persistStream));
      v->vt = VT_UNKNOWN; v->punkVal = transferOwnership.Detach();
    } else if (vt == VT_BSTR) {
      v->vt = VT_BSTR;
			if (size() > 0)
				v->bstrVal = ::SysAllocStringByteLen((const char*)binaryBuffer(), size());
			else
				v->bstrVal = ::SysAllocString(L"");
    } else {
      RINOK(detachPODType(v))
    }

    clear();
    return S_OK;
  }
  HRESULT detach(unsigned char *buffer, size_t bufferSize) {
    if (!buffer)
      return E_POINTER;
    if (VT_EMPTY == vt) {
      memset(buffer, 0, bufferSize);
      return S_OK;
    }
    if (bufferSize < size())
      return S_FALSE;

		memcpy(buffer, binaryBuffer(), bufferSize);
		// return E_NOTIMPL;

    clear();
    return S_OK;
  }
private:
	ScopedComPtr<IPersistStream> persistStream;
  VARTYPE vt;
  unsigned char staticBuffer[sizeof(PROPVARIANT)]; // store POD and scalar values
  unsigned char *dynamicBuffer; // store BSTR, SAFEARRAY, IUnknown - i.e. IPersistentStream, IPersistentStreamInit
  size_t binaryBufferSize;
  size_t dynamicBufferCapacity;

  unsigned char* binaryBuffer() { return binaryBuffer(binaryBufferSize); }
  unsigned char* binaryBuffer(size_t v) {
    if ((v < 1) || (v > 0xACB0000))
      return NULL;

    binaryBufferSize = v; /// in theory, code below contain dynamic allocate && deallocate memory - may throw, then binaryBufferSize become invalid
    if (v < sizeof(staticBuffer))
      return &staticBuffer[0];
    else {
      if (dynamicBuffer && (v > dynamicBufferCapacity)) {
        delete [] dynamicBuffer; dynamicBuffer = NULL;
        dynamicBufferCapacity = 0;
      }
      if (!dynamicBuffer) {
        dynamicBuffer = new unsigned char[v];
        dynamicBufferCapacity = v;
      }

      return dynamicBuffer;
    }
  }
	// rely on byte-to-byte copy assignment used compiler when no explicit assignment operator define
  //VT_I1 cVal
  //VT_UI1 bVal
  //VT_I2 iVal
  //VT_UI2 uiVal
  //VT_I4 lVal
  //VT_UI4 ulVal
  //VT_INT intVal
  //VT_UINT uintVal
  //VT_I8 hVal.QuadPart
  //VT_UI8 uhVal.QuadPart
  //VT_BOOL boolVal
  //VT_R4 fltVal
  //VT_R8 dblVal
  //VT_CY cyVal
  //VT_DATE date
  template<typename _VariantType>
  inline void assignValue(_VariantType const &v) {
		DUMBASS_CHECK(binaryBuffer(sizeof(_VariantType)));

    *((_VariantType*)binaryBuffer(sizeof(_VariantType))) = v;
  }
  HRESULT attachPODType(comUtility::PropVariantRenamed const &v) {
    //return E_NOTIMPL; // check for VT_EMPTY first, then push this method
    //[!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! VT_EMPTY -> binaryBufferSize == 0]
    switch(v.vt) {
			case VT_EMPTY:
				binaryBufferSize = 0;
				break;
      case VT_ERROR:
      case VT_CARRAY:
        return E_FAIL;
      case VT_I1:
        // memcpy(binaryBuffer(), &v.cVal, sizeof(v.cVal));
        assignValue(v.cVal);
        break;
      case VT_UI1:
        assignValue(v.bVal);
        break;
      case VT_I2:
        assignValue(v.iVal);
        break;
      case VT_UI2:
        assignValue(v.uiVal);
        break;
      case VT_I4:
        assignValue(v.lVal);
        break;
      case VT_UI4:
        assignValue(v.ulVal);
        break;
      case VT_INT:
        assignValue(v.intVal);
        break;
      case VT_UINT:
        assignValue(v.uintVal);
        break;
      case VT_I8:
        assignValue(v.hVal.QuadPart);
        break;
      case VT_UI8:
        assignValue(v.uhVal.QuadPart);
        break;
      case VT_BOOL:
        assignValue(v.boolVal);
        break;
      case VT_R4:
        assignValue(v.fltVal);
        break;
      case VT_R8:
        assignValue(v.dblVal);
        break;
      case VT_CY:
        assignValue(v.cyVal);
        break;
      case VT_DATE:
        assignValue(v.date);
        break;
      case VT_FILETIME:
        {
          ULARGE_INTEGER r;
          r.HighPart = v.filetime.dwHighDateTime; r.LowPart = v.filetime.dwLowDateTime;
					assignValue(r.QuadPart);
        }
        break;
      default:
        return E_NOTIMPL;
    }

    vt = v.vt;
    return S_OK;
  }
	template<typename _VariantType>
  inline void copyValue(_VariantType *v) {
		DUMBASS_CHECK(size() == sizeof(_VariantType));

		*v = *((_VariantType*)binaryBuffer());
  }
  HRESULT detachPODType(comUtility::PropVariantRenamed *v) {
		// assume RINOK(v->Clear()) already called in detach
		DUMBASS_CHECK(VT_EMPTY == v->vt);

    switch(vt) {
			case VT_EMPTY:
				// return v->Clear();
				return S_OK; // assumption above
      case VT_ERROR: 
      case VT_CARRAY:
        return E_FAIL;
      case VT_I1:
        copyValue(&v->cVal);
        break;
      case VT_UI1:
        copyValue(&v->bVal);
        break;
      case VT_I2:
        copyValue(&v->iVal);
        break;
      case VT_UI2:
        copyValue(&v->uiVal);
        break;
      case VT_I4:
        copyValue(&v->lVal);
        break;
      case VT_UI4:
        copyValue(&v->ulVal);
        break;
      case VT_INT:
        copyValue(&v->intVal);
        break;
      case VT_UINT:
        copyValue(&v->uintVal);
        break;
      case VT_I8:
        copyValue(&v->hVal.QuadPart);
        break;
      case VT_UI8:
        copyValue(&v->uhVal.QuadPart);
        break;
      case VT_BOOL:
        copyValue(&v->boolVal);
        break;
      case VT_R4:
        copyValue(&v->fltVal);
        break;
      case VT_R8:
        copyValue(&v->dblVal);
        break;
      case VT_CY:
        copyValue(&v->cyVal);
        break;
      case VT_DATE:
        copyValue(&v->date);
        break;
      case VT_FILETIME:
        {
          ULARGE_INTEGER r;
					copyValue(&r.QuadPart);
          v->filetime.dwHighDateTime = r.HighPart; v->filetime.dwLowDateTime = r.LowPart;
        }
        break;
      default:
				return E_NOTIMPL;
		}

		v->vt = vt;
		return S_OK;
  }

	struct clsidLess : public std::binary_function<CLSID, CLSID, bool>
	{	// functor for operator<
	bool operator()(const CLSID &left_, const CLSID &right_) const {
		if (left_.Data1 != right_.Data1) {
			return (left_.Data1 < right_.Data1);
		} else if (left_.Data2 != right_.Data2) {
			return (left_.Data2 < right_.Data2);
		} else if (left_.Data3 != right_.Data3) {
			return (left_.Data3 < right_.Data3);
		} else {
			for (size_t i = 0; i < sizeof(left_.Data4) / sizeof(*left_.Data4); ++i) {
				if (left_.Data4[i] != right_.Data4[i])
					return (left_.Data4[i] < right_.Data4[i]);
			}
		}
		return false;
	}
	};

	typedef std::map<CLSID, cpcl::ComPtr<ICustomClassFactory>, clsidLess> loadFactoryList_t;
	typedef loadFactoryList_t::iterator loadFactoryList_i;
	loadFactoryList_t loadFactoryList;
	HRESULT findFactory(REFCLSID classID, ICustomClassFactory **factory) {
		ScopedComPtr<ICustomClassFactory> r;
		*factory = 0;

		loadFactoryList_i i = loadFactoryList.find(classID);
		if (i != loadFactoryList.end()) {
			r = (*i).second;
		} else {
			RINOK(CustomClassFactoryList::FindFactoryForCLSID(classID, &r))
			loadFactoryList[classID] = r;
		}

		if (r) {
			*factory = r.Detach();
			return S_OK;
		}
		return E_FAIL;
	}
};

/*
binaryVariantBuffer <-> comUtility::PropVariantRenamed
binaryVariantBuffer {
 HRESULT attach(unsigned char const *buffer, size_t bufferSize) {
  clear();
  
  if (!binaryBuffer_)
   return E_OUTOFMEMORY;
 }

 HRESULT detach(unsigned char *buffer, size_t bufferSize); // if (bufferSize < size()) return S_FALSE
}
*/

bool baseVariantItemStorage::idBasketItem(comUtility::PropVariantRenamed const *v, int *r) const {
  itemBasket_const_i i = std::find_if(itemBasket.begin(), itemBasket.end(), baseVariantItemStorage::_Finder(v));
  if (i != itemBasket.end()) {
    *r = (*i).first;
    return true;
  } else
    return false;
}

bool baseVariantItemStorage::indexListItem(int itemId, int *r) const {
  itemBasket_const_i i = itemBasket.find(itemId);
  if (i != itemBasket.end()) {
    int index(0);
    for(itemList_const_i j = itemList.begin(); j != itemList.end(); ++j, ++index) {
      if (*j == (*i).second) {
        *r = index;
        return true;
      }
    }
  }
  return false;
}

void baseVariantItemStorage::clear() {
	for(itemList_i i = itemList.begin(); i != itemList.end(); ++i) 
			delete (*i);
	itemBasket.clear();
  itemList.clear();
}

void baseVariantItemStorage::copy(baseVariantItemStorage const &v) {
  clear();
  for(itemList_const_i i = v.itemList.begin(); i != v.itemList.end(); ++i) {
    itemList.push_back(new comUtility::PropVariantRenamed(*(*i)));
    DUMBASS_CHECK(itemList.back()->vt != VT_ERROR);
    
    int id(0);
    DUMBASS_CHECK(v.idBasketItem((*i), &id));
    itemBasket[id] = itemList.back();
  }
}

comUtility::PropVariantRenamed* baseVariantItemStorage::ensureItem(int itemId) {
  comUtility::PropVariantRenamed *v;

  itemBasket_const_i i = itemBasket.find(itemId);
  if (i != itemBasket.end()) {
    v = (*i).second;
  } else {
    v = new comUtility::PropVariantRenamed();

    itemList.push_back(v);
    itemBasket[itemId] = v;
  }

  return v;
}

size_t const baseVariantItemStorage::magicHeader = 0xAFA11234;
size_t const baseVariantItemStorage::magicItemHeader = 0xBCB0FE12;

/*
baseVariantItemStorage: size_t - magicHeader
baseVariantItemStorage: size_t - items bytes

baseVariantItemStorage: int - itemId
binaryVariantBuffer:    VARTYPE
binaryVariantBuffer:    size_t - item bytes
binaryVariantBuffer:    PROPVARIANT

baseVariantItemStorage: int - itemId
binaryVariantBuffer:    VARTYPE
binaryVariantBuffer:    size_t - item bytes
binaryVariantBuffer:    PROPVARIANT

...

when saving and encounter IPersistStream, save IStream.Tell() and calculate item bytes
after loading item, check IStream.Tell() with item bytes claimed, if not equal - E_UNEXPECTED
*/

//baseVariantItemStorage: size_t - baseVariantItemStorage::magicHeader
//baseVariantItemStorage: size_t - items count
//
//baseVariantItemStorage: size_t - baseVariantItemStorage::magicItemHeader
//baseVariantItemStorage: int - itemId
//binaryVariantBuffer:    VARTYPE is VT_ANY
//binaryVariantBuffer:    size_t - dataSize
//binaryVariantBuffer:    unsigned char* - data
//
//baseVariantItemStorage: size_t - baseVariantItemStorage::magicItemHeader
//baseVariantItemStorage: int - itemId
//binaryVariantBuffer:    VARTYPE is VT_UNKNOWN
//binaryVariantBuffer:    IID - classID
//binaryVariantBuffer:    CreateInstance(classID).Load(IStream*)
//
HRESULT baseVariantItemStorage::load(IStream *v) {
	try {
		if (!v)
			return E_POINTER;

		binaryVariantBuffer buffer;
		cpcl::IStreamWrapper stream(v);
		
		size_t magicHeader_;
		RINOK(streamRead(stream, &magicHeader_))
		if (magicHeader_ != magicHeader)
			return E_UNEXPECTED;
		size_t items_;
		RINOK(streamRead(stream, &items_))
			
		while (items_ > 0) {
			size_t magicItemHeader_;
			RINOK(streamRead(stream, &magicItemHeader_))
			if (magicItemHeader_ != magicItemHeader)
				return E_UNEXPECTED;
			
			int itemId;
			RINOK(streamRead(stream, &itemId))
			RINOK(buffer.load(v))
			comUtility::PropVariantRenamed *item = ensureItem(itemId);
			RINOK(buffer.detach(item))
				
			--items_;
			initializeItem(itemId, *item);
		}

		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

HRESULT baseVariantItemStorage::save(IStream *v) const {
	try {
		if (!v)
			return E_POINTER;

		binaryVariantBuffer buffer;
		cpcl::IStreamWrapper stream(v);
		
		RINOK(streamWrite(stream, magicHeader))
		RINOK(streamWrite(stream, static_cast<size_t>(itemList.size())))
		
		itemList_const_i i = itemList.begin();
		while (i != itemList.end()) {
			RINOK(streamWrite(stream, magicItemHeader))
				
			comUtility::PropVariantRenamed const *item = *i;
			int itemId;
			if (!idBasketItem(item, &itemId))
				return E_UNEXPECTED;
			RINOK(streamWrite(stream, itemId))
			RINOK(buffer.attach(*item))
			RINOK(buffer.save(v))
			
			++i;
		}

		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

HRESULT baseVariantItemStorage::calculateSize(size_t *v) {
	try {
		if (!v)
			return E_POINTER;
		
		*v = 0;
		binaryVariantBuffer buffer;
		size_t v_ = sizeof(magicHeader);
		v_ += sizeof(size_t);
		
		itemList_const_i i = itemList.begin();
		while (i != itemList.end()) {
			v_ += sizeof(magicItemHeader);
			
			comUtility::PropVariantRenamed const *item = *i;
			int itemId;
			if (!idBasketItem(item, &itemId))
				return E_UNEXPECTED;
			v_ += sizeof(itemId);
			RINOK(buffer.attach(*item))
			v_ += buffer.size();
			
			++i;
		}

		*v = v_;
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

/**************************************** IPropertyBasket ****************************************/

STDMETHODIMP baseVariantItemStorage::Clear(void) {
	try {
		clear();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::get_Count(long *count) {
	try {
		if (!count)
			return E_POINTER;

		*count = (long)itemList.size();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::GetItemInfo(long index, BSTR *itemName, int *itemID, VARTYPE *varType) {
	try {
		if (!itemID || !varType || !itemName)
			return E_POINTER;

		*itemID = 0; *varType = VT_ERROR; *itemName = 0;
		itemList_const_i i = itemList.begin();
		for(; index && (i != itemList.end()); --index, ++i);
		
		if ((i != itemList.end()) && (idBasketItem(*i, itemID)))
			*varType = (*i)->vt;
		
		return (*varType != VT_ERROR) ? S_OK : E_INVALIDARG;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::GetItem(int itemID, VARIANT *value) {
	try {
		if (!value)
			return E_POINTER;

		itemBasket_const_i i = itemBasket.find(itemID);
		if (itemBasket.end() == i)
			return E_INVALIDARG;
		
		comUtility::PropVariantRenamed v(*(*i).second);
		DUMBASS_CHECK(v.vt != VT_ERROR);
		
		return v.Detach(value);
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::SetItem(int itemID, VARIANT value) {
	try {
		comUtility::PropVariantRenamed *item = ensureItem(itemID);
		RINOK(item->Copy(&value))

		DUMBASS_CHECK(item->vt != VT_ERROR);
		return (item->vt != VT_ERROR) ? S_OK : E_FAIL;
	} catch (...) {
		return E_FAIL;
	}
}
// if (bufferSize less then needed) { buffer keep untouch; *bufferSize = requiredBufferSize }
STDMETHODIMP baseVariantItemStorage::GetItemValue(int itemID, unsigned char *buffer, int *bufferSize) {
	try {
		if (!buffer || !bufferSize)
			return E_POINTER;

		itemBasket_i i = itemBasket.find(itemID);
		if (itemBasket.end() == i)
			return E_INVALIDARG;
		
		comUtility::PropVariantRenamed *item = (*i).second;
		DUMBASS_CHECK(item->vt != VT_ERROR);
		
		binaryVariantBuffer v;
		RINOK(v.attach(*item))
		size_t bufferSize_ = *bufferSize;
		*bufferSize = v.size();
		return v.detach(buffer, bufferSize_);
	} catch (...) {
		return E_FAIL;
	}
}
//STDMETHODIMP baseVariantItemStorage::SetItemValue(int itemID, const unsigned char *buffer, int bufferSize) {
//	COM_TRY_BEGIN
//
//  comUtility::PropVariantRenamed *item = ensureItem(itemID);
//  RINOK(item->Clear())
//
//  SAFEARRAY *array = ::SafeArrayCreateVector(VT_UI1, 0, bufferSize);
//  if (!array)
//    return E_FAIL;
//  item->vt = VT_ARRAY; item->parray = array;
//
//  unsigned char HUGEP *arrayData;
//  RINOK(::SafeArrayAccessData(array, (void HUGEP**)&arrayData))
//  memcpy(arrayData, buffer, bufferSize);
//  return ::SafeArrayUnaccessData(SAFEARRAY*);
//
//  COM_TRY_END
//}
STDMETHODIMP baseVariantItemStorage::SetItemValue(int itemID, const unsigned char *buffer, int bufferSize) {
	try {
		if (!buffer)
			return E_POINTER;

		comUtility::PropVariantRenamed *item = ensureItem(itemID);
		RINOK(item->Clear())
			
		binaryVariantBuffer variantBuffer;
		RINOK(variantBuffer.attach(buffer, bufferSize))
		return variantBuffer.detach(item);
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::DeleteItem(long index) {
	try {
		itemList_const_i i = itemList.begin();
		for(; index && (i != itemList.end()); --index, ++i);
		if (itemList.end() == i)
			return E_INVALIDARG;
		
		itemBasket_const_i j = std::find_if(itemBasket.begin(), itemBasket.end(), baseVariantItemStorage::_Finder(*i));
		if (itemBasket.end() == j)
			return E_UNEXPECTED;
		
		delete (*i);
		itemList.erase(i);
		itemBasket.erase(j);
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::RemoveItem(int itemID) {
	try {
		itemBasket_const_i j = itemBasket.find(itemID);
		if (itemBasket.end() == j)
			return E_INVALIDARG;
		
		itemList_const_i i = std::find(itemList.begin(), itemList.end(), (*j).second);
		if (itemList.end() == i)
			return E_UNEXPECTED;
		
		delete (*i);
		itemList.erase(i);
		itemBasket.erase(j);
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP baseVariantItemStorage::FindItem(int itemID, long *index) {
	try {
		if (index)
			*index = 0;
		
		int i;
		if (indexListItem(itemID, &i)) {
			if (index)
				*index = i;
			return S_OK;
		} else
			return S_FALSE;
	} catch (...) {
		return E_FAIL;
	}
}
