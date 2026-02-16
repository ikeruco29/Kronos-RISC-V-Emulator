#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>

//![constructor]

CodeEditor::CodeEditor(QWidget *parent, int pSpaces) : QPlainTextEdit(parent)
    , spaces(pSpaces)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    tabspaces = "";
    for (int i = 0; i < spaces; i++)
        tabspaces += " ";
}

//![extraAreaWidth]

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::updateTabSize(){
    QFontMetrics metrics(font());
    setTabStopDistance(spaces * metrics.horizontalAdvance(' '));
}

//![slotUpdateRequest]

//![resizeEvent]

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//![resizeEvent]

void CodeEditor::wheelEvent(QWheelEvent *event)
{
    // Check if ctrl is pressed
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn(1);
        } else {
            zoomOut(1);
        }

        // IMPORTANT!
        // update line number area width as the font size has changed
        updateLineNumberAreaWidth(0);
        updateTabSize();

        event->accept(); // Stop the event so it doesnt do vertical scroll
    } else {
        // If there's no control, act normal
        QPlainTextEdit::wheelEvent(event);
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event){
    // PRESSED "("
    if (event->text() == "(") {
        QTextCursor cursor = textCursor();
        cursor.insertText("()"); // Insert the complete pair
        cursor.movePosition(QTextCursor::Left); // Move cursor between both brackets
        setTextCursor(cursor);
        return; // Return so the editor doesn't insert another
    }

    // PRESSED "{"
    if (event->text() == "{") {
        QTextCursor cursor = textCursor();
        cursor.insertText("{}");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    // PRESSED "
    if(event->text() == "\""){
        QTextCursor cursor = textCursor();
        cursor.insertText("\"\"");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    // PRESSED <
    if(event->text() == "<"){
        QTextCursor cursor = textCursor();
        QString lineText = cursor.block().text(); // Current line text
        if(lineText.contains("#include")){
            cursor.insertText("<>");
            cursor.movePosition(QTextCursor::Left);
            setTextCursor(cursor);
            return;
        }
    }


    bool isTab = (event->key() == Qt::Key_Tab && event->modifiers() == Qt::NoModifier);

    // in some platforms, Shift + Tab is called "backtab"
    bool isBacktab = (event->key() == Qt::Key_Backtab ||
                      (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ShiftModifier)));

    if (isTab || isBacktab) {
        QTextCursor cursor = textCursor();

        // If there's no selection, tab insert normal tabs
        if (!cursor.hasSelection() && isTab) {
            cursor.insertText(tabspaces);
            return;
        }

        // If there's selection or backtab, process by lines
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();

        // Create a cursor to select all lines without lossing the selection
        QTextCursor editCursor(document());
        editCursor.setPosition(start);
        int startBlock = editCursor.blockNumber();
        editCursor.setPosition(end, QTextCursor::KeepAnchor);
        int endBlock = editCursor.blockNumber();

        editCursor.beginEditBlock(); // To make it like a unique "undo" for Ctrl + Z

        for (int i = startBlock; i <= endBlock; ++i) {
            editCursor.setPosition(document()->findBlockByNumber(i).position());

            if (isTab) {
                // indent
                editCursor.insertText(tabspaces);
            } else {
                // De-indent
                for (int j = 0; j < spaces; ++j) {
                    editCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    if (editCursor.selectedText() == " ") {
                        editCursor.removeSelectedText();
                    } else {
                        break; // if there's no space, stop ereasing
                    }
                }
            }
        }
        editCursor.endEditBlock();
        return;
    }



    // PRESSED RETURN
    // This part handles when tabs should be placed automatically, when to open brackets when enter...
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString lineText = cursor.block().text(); // Current line text

        // 1. Calculate current spaces
        QString leadingWhitespace = "";
        for (QChar c : lineText) {
            if (c.isSpace()) leadingWhitespace += c;
            else break;
        }

        // 2. Return between brackets {|}
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        QString leftChar = cursor.selectedText();
        cursor.movePosition(QTextCursor::Right);    // back to center
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QString rightChar = cursor.selectedText();

        if (!rightChar.isEmpty())   // In case it was the last character of the line, it shouldn't jump to the left
            cursor.movePosition(QTextCursor::Left); // back to center

        if (leftChar == "{" && rightChar == "}") {
            cursor.beginEditBlock();
            // New line + previous indent + 4 extra spaces + new line + previous indent
            // TODO: use spaces as tabsize
            cursor.insertText("\n" + leadingWhitespace + tabspaces + "\n" + leadingWhitespace);
            cursor.movePosition(QTextCursor::Up);
            cursor.movePosition(QTextCursor::EndOfLine);
            cursor.endEditBlock();
            setTextCursor(cursor);
            return;
        }

        // 3. Inherit previous line tabs
        cursor.insertText("\n" + leadingWhitespace);
        setTextCursor(cursor);
        return;
    }

    // If its not an special key, let editor handle.
    QPlainTextEdit::keyPressEvent(event);
}

//![cursorPositionChanged]

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::gray);
        lineColor.setAlphaF(0.2);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

//![cursorPositionChanged]

//![extraAreaPaintEvent_0]

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.setFont(this->font());

    //![extraAreaPaintEvent_0]

    //![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    //![extraAreaPaintEvent_1]

    //![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::gray);
            int paddingRight = 5;   // padding between number and the codeline

            painter.drawText(2, top, lineNumberArea->width() - paddingRight, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
