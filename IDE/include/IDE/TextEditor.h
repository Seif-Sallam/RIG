#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <thread>
#include <map>
#include <regex>
#include "imgui.h"

namespace IDE
{
	class TextEditor
	{
	public:
		enum class PaletteIndex
		{
			Default,
			Keyword,
			Number,
			String,
			CharLiteral,
			Punctuation,
			Preprocessor,
			Identifier,
			KnownIdentifier,
			PreprocIdentifier,
			Comment,
			MultiLineComment,
			Background,
			Cursor,
			Selection,
			ErrorMarker,
			Breakpoint,
			BreakpointOutline,
			CurrentLineIndicator,
			CurrentLineIndicatorOutline,
			LineNumber,
			CurrentLineFill,
			CurrentLineFillInactive,
			CurrentLineEdge,
			ErrorMessage,
			BreakpointDisabled,
			UserFunction,
			UserType,
			UniformVariable,
			GlobalVariable,
			LocalVariable,
			FunctionArgument,
			Max
		};

		enum class ShortcutID
		{
			Undo,
			Redo,
			MoveUp,
			SelectUp,
			MoveDown,
			SelectDown,
			MoveLeft,
			SelectLeft,
			MoveWordLeft,
			SelectWordLeft,
			MoveRight,
			SelectRight,
			MoveWordRight,
			SelectWordRight,
			MoveUpBlock,
			SelectUpBlock,
			MoveDownBlock,
			SelectDownBlock,
			MoveTop,
			SelectTop,
			MoveBottom,
			SelectBottom,
			MoveStartLine,
			SelectStartLine,
			MoveEndLine,
			SelectEndLine,
			ForwardDelete,
			ForwardDeleteWord,
			DeleteRight,
			BackwardDelete,
			BackwardDeleteWord,
			DeleteLeft,
			OverwriteCursor,
			Copy,
			Paste,
			Cut,
			SelectAll,
			AutocompleteOpen,
			AutocompleteSelect,
			AutocompleteSelectActive,
			AutocompleteUp,
			AutocompleteDown,
			NewLine,
			Indent,
			Unindent,
			Find,
			Replace,
			FindNext,
			DebugStep,
			DebugStepInto,
			DebugStepOut,
			DebugContinue,
			DebugJumpHere,
			DebugBreakpoint,
			DebugStop,
			Count
		};

		static const int LineNumberSpace = 20;
		static const int DebugDataSpace = 0;

		struct Shortcut
		{
			// 0 - not used, 1 - used
			bool Alt;
			bool Ctrl;
			bool Shift;

			// -1 - not used, everything else: Win32 VK_ code
			int Key1;
			int Key2;

			Shortcut(int vk1 = -1, int vk2 = -2, bool alt = 0, bool ctrl = 0, bool shift = 0)
				: Key1(vk1), Key2(vk2), Alt(alt), Ctrl(ctrl), Shift(shift)
			{
			}
		};

		enum class SelectionMode
		{
			Normal,
			Word,
			Line
		};

		struct Breakpoint
		{
			int mLine;
			bool mEnabled;
			std::string mCondition;

			Breakpoint()
				: mLine(-1), mEnabled(false)
			{
			}
		};

		// Represents a character coordinate from the user's point of view,
		// i. e. consider an uniform grid (assuming fixed-width font) on the
		// screen as it is rendered, and each cell has its own coordinate, starting from 0.
		// Tabs are counted as [1..mTabSize] count empty spaces, depending on
		// how many space is necessary to reach the next tab stop.
		// For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
		// because it is rendered as "    ABC" on the screen.
		struct Coordinates
		{
			int mLine, mColumn;
			Coordinates()
				: mLine(0), mColumn(0)
			{
			}
			Coordinates(int aLine, int aColumn)
				: mLine(aLine), mColumn(aColumn)
			{
				assert(aLine >= -1);
				assert(aColumn >= -1);
			}
			static Coordinates Invalid()
			{
				static Coordinates invalid(-1, -1);
				return invalid;
			}

			bool operator==(const Coordinates &o) const
			{
				return mLine == o.mLine && mColumn == o.mColumn;
			}

			bool operator!=(const Coordinates &o) const
			{
				return mLine != o.mLine || mColumn != o.mColumn;
			}

			bool operator<(const Coordinates &o) const
			{
				if (mLine != o.mLine)
					return mLine < o.mLine;
				return mColumn < o.mColumn;
			}

			bool operator>(const Coordinates &o) const
			{
				if (mLine != o.mLine)
					return mLine > o.mLine;
				return mColumn > o.mColumn;
			}

			bool operator<=(const Coordinates &o) const
			{
				if (mLine != o.mLine)
					return mLine < o.mLine;
				return mColumn <= o.mColumn;
			}

			bool operator>=(const Coordinates &o) const
			{
				if (mLine != o.mLine)
					return mLine > o.mLine;
				return mColumn >= o.mColumn;
			}
		};

		struct Identifier
		{
			Identifier() {}
			Identifier(const std::string &declr)
				: mDeclaration(declr)
			{
			}

			Coordinates mLocation;
			std::string mDeclaration;
		};

		typedef std::string String;
		typedef std::unordered_map<std::string, Identifier> Identifiers;
		typedef std::unordered_set<std::string> Keywords;
		typedef std::map<int, std::string> ErrorMarkers;
		typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;
		typedef uint8_t Char;

		struct Glyph
		{
			Char mChar;
			PaletteIndex mColorIndex = PaletteIndex::Default;
			bool mComment : 1;
			bool mMultiLineComment : 1;
			bool mPreprocessor : 1;

			Glyph(Char aChar, PaletteIndex aColorIndex)
				: mChar(aChar), mColorIndex(aColorIndex), mComment(false), mMultiLineComment(false), mPreprocessor(false)
			{
			}
		};

		typedef std::vector<Glyph> Line;
		typedef std::vector<Line> Lines;

		struct LanguageDefinition
		{
			typedef std::pair<std::string, PaletteIndex> TokenRegexString;
			typedef std::vector<TokenRegexString> TokenRegexStrings;
			typedef bool (*TokenizeCallback)(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end, PaletteIndex &paletteIndex);

			std::string mName;
			Keywords mKeywords;
			Identifiers mIdentifiers;
			Identifiers mPreprocIdentifiers;
			std::string mCommentStart, mCommentEnd, mSingleLineComment;
			char mPreprocChar;
			bool mAutoIndentation;

			TokenizeCallback mTokenize;

			TokenRegexStrings mTokenRegexStrings;

			bool mCaseSensitive;

			LanguageDefinition()
				: mPreprocChar('#'), mAutoIndentation(true), mTokenize(nullptr), mCaseSensitive(true)
			{
			}

			static const LanguageDefinition &CPlusPlus();
			static const LanguageDefinition &RISCV();
			static const LanguageDefinition &C();
		};

		TextEditor();
		~TextEditor() = default;


		bool ReadFile(const std::string& filePath);
		void SetLanguageDefinition(const LanguageDefinition &aLanguageDef);
		const LanguageDefinition &GetLanguageDefinition() const { return mLanguageDefinition; }

		const Palette &GetPalette() const { return mPaletteBase; }
		void SetPalette(const Palette &aValue);

		void SetErrorMarkers(const ErrorMarkers &aMarkers) { mErrorMarkers = aMarkers; }

		bool HasBreakpoint(int line);
		void AddBreakpoint(int line, std::string condition = "", bool enabled = true);
		void RemoveBreakpoint(int line);
		void SetBreakpointEnabled(int line, bool enable);
		Breakpoint &GetBreakpoint(int line);
		inline const std::vector<Breakpoint> &GetBreakpoints() { return mBreakpoints; }
		void SetCurrentLineIndicator(int line);

		inline bool IsDebugging() { return mDebugCurrentLine > 0; }

		void Render(const char *aTitle, const ImVec2 &aSize = ImVec2(), bool aBorder = false);
		void SetText(const std::string &aText);
		std::string GetText() const;

		void SetTextLines(const std::vector<std::string> &aLines);
		void GetTextLines(std::vector<std::string> &out) const;

		std::string GetSelectedText() const;
		std::string GetCurrentLineText() const;

		int GetTotalLines() const { return (int)mLines.size(); }
		bool IsOverwrite() const { return mOverwrite; }

		bool IsFocused() const { return mFocused; }
		void SetReadOnly(bool aValue);
		bool IsReadOnly() { return mReadOnly || IsDebugging(); }
		bool IsTextChanged() const { return mTextChanged; }
		bool IsCursorPositionChanged() const { return mCursorPositionChanged; }
		inline void ResetTextChanged() { mTextChanged = false; }

		bool IsColorizerEnabled() const { return mColorizerEnabled; }
		void SetColorizerEnable(bool aValue);

		Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }
		void SetCursorPosition(const Coordinates &aPosition);

		inline void SetHandleMouseInputs(bool aValue) { mHandleMouseInputs = aValue; }
		inline bool IsHandleMouseInputsEnabled() const { return mHandleKeyboardInputs; }

		inline void SetHandleKeyboardInputs(bool aValue) { mHandleKeyboardInputs = aValue; }
		inline bool IsHandleKeyboardInputsEnabled() const { return mHandleKeyboardInputs; }

		inline void SetImGuiChildIgnored(bool aValue) { mIgnoreImGuiChild = aValue; }
		inline bool IsImGuiChildIgnored() const { return mIgnoreImGuiChild; }

		inline void SetShowWhitespaces(bool aValue) { mShowWhitespaces = aValue; }
		inline bool IsShowingWhitespaces() const { return mShowWhitespaces; }

		void InsertText(const std::string &aValue, bool indent = false);
		void InsertText(const char *aValue, bool indent = false);

		void MoveUp(int aAmount = 1, bool aSelect = false);
		void MoveDown(int aAmount = 1, bool aSelect = false);
		void MoveLeft(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
		void MoveRight(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
		void MoveTop(bool aSelect = false);
		void MoveBottom(bool aSelect = false);
		void MoveHome(bool aSelect = false);
		void MoveEnd(bool aSelect = false);

		void SetSelectionStart(const Coordinates &aPosition);
		void SetSelectionEnd(const Coordinates &aPosition);
		void SetSelection(const Coordinates &aStart, const Coordinates &aEnd, SelectionMode aMode = SelectionMode::Normal);
		void SelectWordUnderCursor();
		void SelectAll();
		bool HasSelection() const;

		void Copy();
		void Cut();
		void Paste();
		void Delete();

		bool CanUndo();
		bool CanRedo();
		void Undo(int aSteps = 1);
		void Redo(int aSteps = 1);

		inline void SetTabSize(int s) { mTabSize = std::max<int>(0, std::min<int>(32, s)); }
		inline int GetTabSize() { return mTabSize; }

		inline void SetInsertSpaces(bool s) { mInsertSpaces = s; }
		inline int GetInsertSpaces() { return mInsertSpaces; }

		inline void SetSmartIndent(bool s) { mSmartIndent = s; }
		inline void SetAutoIndentOnPaste(bool s) { mAutoindentOnPaste = s; }
		inline void SetHighlightLine(bool s) { mHighlightLine = s; }
		inline void SetCompleteBraces(bool s) { mCompleteBraces = s; }
		inline void SetHorizontalScroll(bool s) { mHorizontalScroll = s; }
		inline void SetSmartPredictions(bool s) { mAutocomplete = s; }
		inline void SetFunctionTooltips(bool s) { mFuncTooltips = s; }
		inline void SetActiveAutocomplete(bool cac) { mActiveAutocomplete = cac; }
		inline void SetScrollbarMarkers(bool markers) { mScrollbarMarkers = markers; }
		inline void SetSidebarVisible(bool s) { mSidebar = s; }
		inline void SetSearchEnabled(bool s) { mHasSearch = s; }

		inline void SetUIScale(float scale) { mUIScale = scale; }
		inline void SetUIFontSize(float size) { mUIFontSize = size; }
		inline void SetEditorFontSize(float size) { mEditorFontSize = size; }

		void SetShortcut(TextEditor::ShortcutID id, Shortcut s);

		inline void SetShowLineNumbers(bool s)
		{
			mShowLineNumbers = s;
			mTextStart = (s ? 20.0f : 6.0f);
			mLeftMargin = (s ? (DebugDataSpace + LineNumberSpace) : (DebugDataSpace - LineNumberSpace));
		}
		inline int GetTextStart() const { return mShowLineNumbers ? 7 : 3; }

		void Colorize(int aFromLine = 0, int aCount = -1);
		void ColorizeRange(int aFromLine = 0, int aToLine = 0);
		void ColorizeInternal();

		struct FunctionData
		{
			FunctionData()
			{
				LineStart = -1;
				LineEnd = -1;
			}
			FunctionData(int lineStart, int lineEnd, const std::vector<std::string> &args, const std::vector<std::string> &locals)
			{
				LineStart = lineStart;
				LineEnd = lineEnd;
				Arguments = args;
				Locals = locals;
			}
			int LineStart;
			int LineEnd;
			std::vector<std::string> Arguments;
			std::vector<std::string> Locals;
		};
		inline void ClearAutocompleteData()
		{
			mACFunctions.clear();
			mACUserTypes.clear();
			mACUniforms.clear();
			mACGlobals.clear();
		}
		inline void ClearAutocompleteEntries()
		{
			mACEntries.clear();
			mACEntrySearch.clear();
		}
		inline const std::unordered_map<std::string, FunctionData> &GetAutocompleteFunctions() { return mACFunctions; }
		inline const std::vector<std::string> &GetAutocompleteUserTypes() { return mACUserTypes; }
		inline const std::vector<std::string> &GetAutocompleteUniforms() { return mACUniforms; }
		inline const std::vector<std::string> &GetAutocompleteGlobals() { return mACGlobals; }
		inline void AddAutocompleteFunction(const std::string &fname, int lineStart, int lineEnd, const std::vector<std::string> &args, const std::vector<std::string> &locals)
		{
			mACFunctions[fname] = FunctionData(lineStart, lineEnd, args, locals);
		}
		inline void AddAutocompleteUserType(const std::string &fname)
		{
			mACUserTypes.push_back(fname);
		}
		inline void AddAutocompleteUniform(const std::string &fname)
		{
			mACUniforms.push_back(fname);
		}
		inline void AddAutocompleteGlobal(const std::string &fname)
		{
			mACGlobals.push_back(fname);
		}
		inline void AddAutocompleteEntry(const std::string &search, const std::string &display, const std::string &value)
		{
			mACEntrySearch.push_back(search);
			mACEntries.push_back(std::make_pair(display, value));
		}

		static const std::vector<Shortcut> GetDefaultShortcuts();
		static const Palette &GetDarkPalette();
		static const Palette &GetLightPalette();
		static const Palette &GetRetroBluePalette();

		enum class DebugAction
		{
			Step,
			StepInto,
			StepOut,
			Continue,
			Stop
		};

		std::function<void(TextEditor *, int)> OnDebuggerJump;
		std::function<void(TextEditor *, DebugAction)> OnDebuggerAction;
		std::function<void(TextEditor *, const std::string &)> OnIdentifierHover;
		std::function<bool(TextEditor *, const std::string &)> HasIdentifierHover;
		std::function<void(TextEditor *, const std::string &)> OnExpressionHover;
		std::function<bool(TextEditor *, const std::string &)> HasExpressionHover;
		std::function<void(TextEditor *, int)> OnBreakpointRemove;
		std::function<void(TextEditor *, int, const std::string &, bool)> OnBreakpointUpdate;

		std::function<void(TextEditor *)> OnContentUpdate;

		inline void SetCurrentOpenedPath(const std::string &path) { mPath = path; }
		inline const std::string &GetCurrentOpenedPath() { return mPath; }

	private:
		std::string mPath;

		typedef std::vector<std::pair<std::regex, PaletteIndex>> RegexList;

		struct EditorState
		{
			Coordinates mSelectionStart;
			Coordinates mSelectionEnd;
			Coordinates mCursorPosition;
		};

		class UndoRecord
		{
		public:
			UndoRecord() {}
			~UndoRecord() {}

			UndoRecord(
				const std::string &aAdded,
				const TextEditor::Coordinates aAddedStart,
				const TextEditor::Coordinates aAddedEnd,

				const std::string &aRemoved,
				const TextEditor::Coordinates aRemovedStart,
				const TextEditor::Coordinates aRemovedEnd,

				TextEditor::EditorState &aBefore,
				TextEditor::EditorState &aAfter);

			void Undo(TextEditor *aEditor);
			void Redo(TextEditor *aEditor);

			std::string mAdded;
			Coordinates mAddedStart;
			Coordinates mAddedEnd;

			std::string mRemoved;
			Coordinates mRemovedStart;
			Coordinates mRemovedEnd;

			EditorState mBefore;
			EditorState mAfter;
		};

		typedef std::vector<UndoRecord> UndoBuffer;

		void ProcessInputs();
		float TextDistanceToLineStart(const Coordinates &aFrom) const;
		void EnsureCursorVisible();
		int GetPageSize() const;
		std::string GetText(const Coordinates &aStart, const Coordinates &aEnd) const;
		Coordinates GetActualCursorCoordinates() const;
		Coordinates SanitizeCoordinates(const Coordinates &aValue) const;
		void Advance(Coordinates &aCoordinates) const;
		void DeleteRange(const Coordinates &aStart, const Coordinates &aEnd);
		int InsertTextAt(Coordinates &aWhere, const char *aValue, bool indent = false);
		void AddUndo(UndoRecord &aValue);
		Coordinates ScreenPosToCoordinates(const ImVec2 &aPosition) const;
		Coordinates MousePosToCoordinates(const ImVec2 &aPosition) const;
		ImVec2 CoordinatesToScreenPos(const TextEditor::Coordinates &aPosition) const;
		Coordinates FindWordStart(Coordinates aFrom) const;
		Coordinates FindWordEnd(Coordinates aFrom) const;
		Coordinates FindNextWord(Coordinates aFrom) const;
		int GetCharacterIndex(const Coordinates &aCoordinates) const;
		int GetCharacterColumn(int aLine, int aIndex) const;
		int GetLineCharacterCount(int aLine) const;
		int GetLineMaxColumn(int aLine) const;
		bool IsOnWordBoundary(const Coordinates &aAt) const;
		void RemoveLine(int aStart, int aEnd);
		void RemoveLine(int aIndex);
		Line &InsertLine(int aIndex);
		void EnterCharacter(ImWchar aChar, bool aShift);
		void Backspace();
		void DeleteSelection();
		std::string GetWordUnderCursor() const;
		std::string GetWordAt(const Coordinates &aCoords) const;
		ImU32 GetGlyphColor(const Glyph &aGlyph) const;

		void HandleKeyboardInputs();
		void HandleMouseInputs();
		void RenderInternal(const char *aTitle);

		bool mFuncTooltips;

		float mUIScale, mUIFontSize, mEditorFontSize;
		inline float mUICalculateSize(float h)
		{
			return h * (mUIScale + mUIFontSize / 16.f - 1.0f);
		}
		inline float mEditorCalculateSize(float h)
		{
			return h * (mUIScale + mEditorFontSize / 16.f - 1.0f);
		}

		float mLineSpacing;
		Lines mLines;
		EditorState mState;
		UndoBuffer mUndoBuffer;
		int mUndoIndex;
		int mReplaceIndex;

		float mFontSize;

		bool mSidebar;
		bool mHasSearch;

		char mFindWord[256];
		bool mFindOpened;
		bool mFindJustOpened;
		bool mFindNext;
		bool mFindFocused, mReplaceFocused;
		bool mReplaceOpened;
		char mReplaceWord[256];

		std::vector<std::string> mACEntrySearch;
		std::vector<std::pair<std::string, std::string>> mACEntries;

		bool mIsSnippet;
		std::vector<Coordinates> mSnippetTagStart, mSnippetTagEnd;
		std::vector<int> mSnippetTagID;
		std::vector<bool> mSnippetTagHighlight;
		int mSnippetTagSelected, mSnippetTagLength, mSnippetTagPreviousLength;
		std::string mAutcompleteParse(const std::string &str, const Coordinates &start);
		void mAutocompleteSelect();

		bool m_requestAutocomplete, m_readyForAutocomplete;
		void m_buildSuggestions(bool *keepACOpened = nullptr);
		bool mActiveAutocomplete;
		bool mAutocomplete;
		std::unordered_map<std::string, FunctionData> mACFunctions;
		std::vector<std::string> mACUserTypes, mACUniforms, mACGlobals;
		std::string mACWord;
		std::vector<std::pair<std::string, std::string>> mACSuggestions;
		int mACIndex;
		bool mACOpened;
		bool mACSwitched; // if == true then allow selection with enter
		Coordinates mACPosition;

		std::vector<Shortcut> mShortcuts;

		bool mScrollbarMarkers;
		bool mHorizontalScroll;
		bool mCompleteBraces;
		bool mShowLineNumbers;
		bool mHighlightLine;
		bool mInsertSpaces;
		bool mSmartIndent;
		bool mFocused;
		int mTabSize;
		bool mOverwrite;
		bool mReadOnly;
		bool mWithinRender;
		bool mScrollToCursor;
		bool mScrollToTop;
		bool mTextChanged;
		bool mColorizerEnabled;
		float mTextStart; // position (in pixels) where a code line starts relative to the left of the TextEditor.
		int mLeftMargin;
		bool mCursorPositionChanged;
		int mColorRangeMin, mColorRangeMax;
		SelectionMode mSelectionMode;
		bool mHandleKeyboardInputs;
		bool mHandleMouseInputs;
		bool mIgnoreImGuiChild;
		bool mShowWhitespaces;
		bool mAutoindentOnPaste;

		Palette mPaletteBase;
		Palette mPalette;
		LanguageDefinition mLanguageDefinition;
		RegexList mRegexList;

		float mDebugBarWidth, mDebugBarHeight;

		bool mDebugCurrentLineUpdated;
		int mDebugCurrentLine;
		ImVec2 mUICursorPos, mFindOrigin;
		float mWindowWidth;
		std::vector<Breakpoint> mBreakpoints;
		ImVec2 mRightClickPos;

		int mPopupCondition_Line;
		bool mPopupCondition_Use;
		char mPopupCondition_Condition[512];

		bool mCheckComments;
		ErrorMarkers mErrorMarkers;
		ImVec2 mCharAdvance;
		Coordinates mInteractiveStart, mInteractiveEnd;
		std::string mLineBuffer;
		uint64_t mStartTime;

		Coordinates mLastHoverPosition;
		std::chrono::steady_clock::time_point mLastHoverTime;

		float mLastClick;
	};
}