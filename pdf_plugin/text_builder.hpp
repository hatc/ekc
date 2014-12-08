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
#include <algorithm>

namespace text2text {

template<class _NodeType>
inline _NodeType selector(_NodeType v) { return (_NodeType)0; }
inline TextWord*  selector(TextLine *v) { return v->words; }
inline TextLine*  selector(TextBlock *v) { return v->lines; }
inline TextBlock* selector(TextFlow *v) { return v->blocks; }
inline TextFlow*  selector(TextPage *v) { return v->flows; }

template<class _NodeType>
struct selector_type { typedef _NodeType child_type; };
template<>
struct selector_type<TextLine*>		{ typedef TextWord *child_type;  };
template<>
struct selector_type<TextBlock*>	{ typedef TextLine *child_type;  };
template<>
struct selector_type<TextFlow*>		{ typedef TextBlock *child_type; };

template<class _NodeType>
inline bool is_line(_NodeType) { return false; }
inline bool is_line(TextLine*) { return true; }
template<class _NodeType>
inline bool is_hyphen_line(_NodeType) { return false; }
inline bool is_hyphen_line(TextLine *line) { return gTrue == line->hyphenated; }

template<class _NodeType>
inline int length(_NodeType) { return -1; }
inline int length(TextWord *word) { return word->getLength(); }
template<class _NodeType>
inline RectContainer::Rect rect(_NodeType, int) { return RectContainer::Rect(); }
inline RectContainer::Rect rect(TextWord *word, int i) { return ((TextWordEx*)word)->rects[i]; }
template<class _NodeType>
inline std::wstring text(_NodeType, int) { return std::wstring(); }
inline std::wstring text(TextWord *word, int i) { 
  std::wstring r; r.reserve(i);
  for(int j = 0; j < i; ++j) {
    if (word->text[j] >> 8) {
			r.append(1, word->text[j]);
		} else {
			r.append(1, (wchar_t)cpcl::CP1251_UTF16((unsigned char)word->text[j]));
    }
  }
  return r;
}
inline bool rotated(TextWord *v) { return v->rot & 1; }
template<class _NodeType>
inline bool breakWord(_NodeType, _NodeType, double) { return false; }
inline bool breakWord(TextWord *word, TextWord *word_prev, double minWordSpacing_) {
	if (word_prev) {
		if (rotated(word)) {
			return (word->yMax < (word_prev->yMin - (minWordSpacing_ * word_prev->fontSize)));
		} else {
			return (word->xMin > (word_prev->xMax + (minWordSpacing_ * word_prev->fontSize)));
		}
	}
	
	return false;
}
template<class _NodeType>
inline bool isHyphenChar(_NodeType) { return false; }
inline bool isHyphenChar(TextWord *word) {
	return is_hyphen((wchar_t)word->text[word->getLength() - 1]);
}

inline void drawWord(RectContainer &container, TextWord *word, int length, /* UnicodeMap *uMap, */ bool state) {
	if (length > word->getLength())
		return;

	for(int j = 0; j < length; ++j) {
    if ((state) && ((length - 1) == j)) {
      RectContainer::Rect l = ((TextWordEx*)word)->rects[j];
      RectContainer::Rect r = ((TextWordEx*)word)->rects[word->getLength() - 1];
			// in djvu_page rect.y1 = z_->rect.ymin && rect.y2 = z_->rect.ymax, so
			l.x1 = (std::min)(l.x1, r.x1); l.y1 = (std::min)(l.y1, r.y1);
      l.x2 = (std::max)(l.x2, r.x2); l.y2 = (std::max)(l.y2, r.y2);

      container.SetRect(l);
    } else {
      container.SetRect(((TextWordEx*)word)->rects[j]);
    }

		/*char buf[0x10] = { 0 };
		int len = uMap->mapUnicode(word->text[j], &buf[0], sizeof(buf));*/
		if (word->text[j] >> 8) {
			container.text.append(1, word->text[j]);
		} else {
			container.text.append(1, (wchar_t)cpcl::CP1251_UTF16((unsigned char)word->text[j]));
		}
	}
}
template<class _NodeType>
inline void draw(RectContainer&, _NodeType, int, /* UnicodeMap*, */ bool) {}
inline void draw(RectContainer &container, TextWord *word, int length, /* UnicodeMap *uMap, */ bool state) {
	drawWord(container, word, length, /*uMap,*/ state);
}
template<class _NodeType>
inline void drawChar(RectContainer&, _NodeType, char, RectContainer::Rect/*, UnicodeMap* */) {}
inline void drawChar(RectContainer &container, TextWord *word, Unicode v, RectContainer::Rect r/*, UnicodeMap *uMap */) { 
	/*char buf[0x10] = {0};
	int len = uMap->mapUnicode(v, &buf[0], sizeof(buf));*/

	container.SetRect(r);
	if (v >> 8) {
		//0x96 U+2013 EN DASH
		if (0x96 == v)
			container.text.append(1, (wchar_t)0x2013);
		else
			container.text.append(1, (wchar_t)v);
	} else {
		container.text.append(1, (wchar_t)cpcl::CP1251_UTF16((unsigned char)v));
	}
}
template<class _NodeType>
inline void drawSpecialChar(RectContainer &container, _NodeType, char v) {
  /*char c; Unicode u;
  u=c=v; container.drawChar(u,&c,1);*/
	container.text.append(1, (wchar_t)v);
}

struct textBuilder {
	enum HyphenatedState { notInDictionary, inHyphenDictionary, notInterestingWord /*inErrorDictionary*/ };

	RectContainer &container_;

	void addText(std::vector<std::vector<TextFlow*>> &flows_clusters) {
		hyphenState = false;
		/*uMap = globalParams->getTextEncoding();*/

		for (std::vector<std::vector<TextFlow*>>::iterator i = flows_clusters.begin(); i != flows_clusters.end(); ++i) {
		for (std::vector<TextFlow*>::iterator flow = (*i).begin(); flow != (*i).end(); ++flow) {
			visit((*flow), selector_type<TextFlow*>::child_type());
		}
		}

		drawSpecialChar(container_, (TextWord*)0, 13);
		drawSpecialChar(container_, (TextWord*)0, 10);

		/*uMap->decRefCnt();
		uMap = (UnicodeMap*)0;*/
	}
	HyphenatedState checkWord(std::wstring const &v) {
		std::wstring word = wstr_trim(v);
		if (!wstr_interestingWord(word))
			return notInterestingWord;

    wstr_lower(word);
		if (globalParams->isHyphenWord(word))
			return inHyphenDictionary;
		else if (globalParams->isErrorWord(word))
			return notInterestingWord;

		return notInDictionary;
	}

	textBuilder(RectContainer &container) : hyphenState(false), container_(container) /*, uMap((UnicodeMap*)0)*/ {}
private:
	bool hyphenState;
	std::wstring wordText;
	RectContainer::Rect hyphenSymbolRect;

	/*UnicodeMap *uMap;*/
	
	//template<class _NodeType>
	//void visit(_NodeType node) {
	//	_NodeType prev = (_NodeType)0;
	//	for (_NodeType n = node; n; n = n->next) {
	//		if (selector(n)) { // has childs
	//			visit(selector(n));
	//		if (is_line(n)) {
	//				aprioHypen = is_hyphen_line(n);
	//			}
	//			if (!hyphenState) {
	//				checkWord(currentWordText);
	//				currentWordText.clear();
	//			}
	//		} else { // no childs - TextWord
	//			if (!n->next) && ()
	//			if (breakWord(n,prev)) {
	//				checkWord(currentWordText, hyphenState);
	//				hyphenState = false;
	//				currentWordText.clear();
	//			}
	//			//currentWordText += data_text(n);
	//		}
	//		prev = n;
	//	}
	//}
	template<class _ParentNodeType, class _NodeType>
	void visit(_ParentNodeType node, _NodeType) {
		_NodeType prev = (_NodeType)0;
    bool drawLineBreak(false);
		for (_NodeType n = selector(node); n; n = n->next) {
			if (selector(n)) { // has childs
				// #not TextWord
				visit(n, selector_type<_NodeType>::child_type());

				if ((is_line(n)) && (!hyphenState)) {
					// break line - draw(\x0D\x0A);
					drawSpecialChar(container_, n, 13);
					drawSpecialChar(container_, n, 10);
				}
			} else { // no childs - TextWord
				// #TextWord
				int len = length(n);
        bool drawWord(true);
				if (hyphenState) {
					hyphenState = false;
					HyphenatedState v = notInDictionary;

					std::wstring secondPartHyphenWord = text(n, len);
					//if ((checkWord(wordText) == notInterestingWord)
					//	|| (checkWord(secondPartHyphenWord) == notInterestingWord)) {
					//	v = notInterestingWord;
					//} else {
					//	v = checkWord((wordText + L'-') + secondPartHyphenWord);
					//}
          v = checkWord((wordText + L'-') + secondPartHyphenWord);

					switch(v) {
						case notInDictionary:
							// remove line break and hyphen symbol
							draw(container_, n, len, /*uMap,*/ hyphenState);
              drawLineBreak = true;
							break;
						case inHyphenDictionary:
							// remove line break but keep hyphen symbol
							drawChar(container_, n, 0x2D, hyphenSymbolRect /*,uMap*/);
							draw(container_, n, len, /*uMap,*/ hyphenState);
              drawLineBreak = true;
							break;
						case notInterestingWord:
						default:
							// keep line break and hyphen symbol
              drawChar(container_, n, 0x2D, hyphenSymbolRect /*,uMap*/);
							drawSpecialChar(container_, n, 13);
							drawSpecialChar(container_, n, 10);
							draw(container_, n, len, /*uMap,*/ hyphenState);
							break;
          }
          drawWord = false;
				} else {
					if ((!n->next) && (is_hyphen_line(node))) {
						hyphenState = true;
						if (isHyphenChar(n)) {
							hyphenSymbolRect = rect(n, --len);
						}
					}					
				}

				if (breakWord(n, prev, minWordSpacing)) {
					// break word - draw(\x20);
          if (drawLineBreak) {
            drawSpecialChar(container_, n, 13);
            drawSpecialChar(container_, n, 10);
            drawLineBreak = false;
          } else {
            if ((length(n) == 1) && (length(prev) == 1)) {
              if (breakWord(n, prev, 0.3))
                drawSpecialChar(container_, n, 32);
            } else {
              drawSpecialChar(container_, n, 32);
            }
          }
				}

        wordText = text(n, len);
        if (drawWord) {
          draw(container_, n, len, /*uMap,*/ hyphenState);
        }
			}

			prev = n;
		}
	}

	textBuilder(textBuilder const &);
	textBuilder& operator=(textBuilder const &);
};

};
