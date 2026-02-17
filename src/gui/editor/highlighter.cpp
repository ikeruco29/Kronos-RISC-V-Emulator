#include "gui/editor/highlighter.h"

Highlighter::Highlighter(QTextDocument *parent, int language_param)
    : QSyntaxHighlighter(parent), language(language_param)
{
    HighlightingRule rule;
    // regex for variables keywords
    keywordFormat.setForeground(QColor(187, 154, 247));
    keywordFormat.setFontWeight(QFont::Bold);

    const QString keywordPatternsC[] = {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\binclude\\b"), QStringLiteral("\\bdefine\\b")
    };

    const QString keywordPatternsAssembly[] = {

    /* ===== RV32I base ===== */
        QStringLiteral("\\badd\\b"), QStringLiteral("\\bsub\\b"),
        QStringLiteral("\\bsll\\b"), QStringLiteral("\\bslt\\b"), QStringLiteral("\\bsltu\\b"),
        QStringLiteral("\\bxor\\b"), QStringLiteral("\\bsrl\\b"), QStringLiteral("\\bsra\\b"),
        QStringLiteral("\\bor\\b"),  QStringLiteral("\\band\\b"),

        QStringLiteral("\\baddi\\b"), QStringLiteral("\\bslti\\b"), QStringLiteral("\\bsltiu\\b"),
        QStringLiteral("\\bxori\\b"), QStringLiteral("\\bori\\b"),  QStringLiteral("\\bandi\\b"),
        QStringLiteral("\\bslli\\b"), QStringLiteral("\\bsrli\\b"), QStringLiteral("\\bsrai\\b"),

        QStringLiteral("\\blb\\b"), QStringLiteral("\\blh\\b"), QStringLiteral("\\blw\\b"),
        QStringLiteral("\\blbu\\b"), QStringLiteral("\\blhu\\b"),

        QStringLiteral("\\bsb\\b"), QStringLiteral("\\bsh\\b"), QStringLiteral("\\bsw\\b"),

        QStringLiteral("\\bbeq\\b"), QStringLiteral("\\bbne\\b"),
        QStringLiteral("\\bble\\b"),
        QStringLiteral("\\bblt\\b"), QStringLiteral("\\bbge\\b"),
        QStringLiteral("\\bbltu\\b"), QStringLiteral("\\bbgeu\\b"),

        QStringLiteral("\\bjal\\b"), QStringLiteral("\\bjalr\\b"),
        QStringLiteral("\\blui\\b"), QStringLiteral("\\bauipc\\b"),

        QStringLiteral("\\becall\\b"), QStringLiteral("\\bebreak\\b"),

        /* ===== RV32M ===== */
        QStringLiteral("\\bmul\\b"), QStringLiteral("\\bmulh\\b"),
        QStringLiteral("\\bmulhsu\\b"), QStringLiteral("\\bmulhu\\b"),
        QStringLiteral("\\bdiv\\b"), QStringLiteral("\\bdivu\\b"),
        QStringLiteral("\\brem\\b"), QStringLiteral("\\bremu\\b"),

        /* ===== RV32A ===== */
        QStringLiteral("\\blr\\.w\\b"), QStringLiteral("\\bsc\\.w\\b"),
        QStringLiteral("\\bamoswap\\.w\\b"), QStringLiteral("\\bamoadd\\.w\\b"),
        QStringLiteral("\\bamoxor\\.w\\b"), QStringLiteral("\\bamoand\\.w\\b"),
        QStringLiteral("\\bamoor\\.w\\b"), QStringLiteral("\\bamomin\\.w\\b"),
        QStringLiteral("\\bamomax\\.w\\b"),
        QStringLiteral("\\bamominu\\.w\\b"), QStringLiteral("\\bamomaxu\\.w\\b"),

        /* ===== RV32F ===== */
        QStringLiteral("\\bflw\\b"), QStringLiteral("\\bfsw\\b"),
        QStringLiteral("\\bfadd\\.s\\b"), QStringLiteral("\\bfsub\\.s\\b"),
        QStringLiteral("\\bfmul\\.s\\b"), QStringLiteral("\\bfdiv\\.s\\b"),
        QStringLiteral("\\bfsqrt\\.s\\b"),
        QStringLiteral("\\bfmin\\.s\\b"), QStringLiteral("\\bfmax\\.s\\b"),
        QStringLiteral("\\bfeq\\.s\\b"), QStringLiteral("\\bflt\\.s\\b"),
        QStringLiteral("\\bfle\\.s\\b"),
        QStringLiteral("\\bfsgnj\\.s\\b"), QStringLiteral("\\bfsgnjn\\.s\\b"),
        QStringLiteral("\\bfsgnjx\\.s\\b"),
        QStringLiteral("\\bfcvt\\.w\\.s\\b"), QStringLiteral("\\bfcvt\\.s\\.w\\b"),
        QStringLiteral("\\bfmv\\.x\\.w\\b"), QStringLiteral("\\bfmv\\.w\\.x\\b"),

        /* ===== RV32D ===== */
        QStringLiteral("\\bfld\\b"), QStringLiteral("\\bfsd\\b"),
        QStringLiteral("\\bfadd\\.d\\b"), QStringLiteral("\\bfsub\\.d\\b"),
        QStringLiteral("\\bfmul\\.d\\b"), QStringLiteral("\\bfdiv\\.d\\b"),
        QStringLiteral("\\bfsqrt\\.d\\b"),
        QStringLiteral("\\bfcvt\\.w\\.d\\b"), QStringLiteral("\\bfcvt\\.d\\.w\\b"),

        /* ===== RV32C (compressed) ===== */
        QStringLiteral("\\bc\\.add\\b"), QStringLiteral("\\bc\\.sub\\b"),
        QStringLiteral("\\bc\\.lw\\b"),  QStringLiteral("\\bc\\.sw\\b"),
        QStringLiteral("\\bc\\.j\\b"),   QStringLiteral("\\bc\\.jal\\b"),
        QStringLiteral("\\bc\\.beqz\\b"), QStringLiteral("\\bc\\.bnez\\b"),
        QStringLiteral("\\bc\\.li\\b"),  QStringLiteral("\\bc\\.lui\\b"),
        QStringLiteral("\\bc\\.mv\\b"),  QStringLiteral("\\bc\\.nop\\b"),

        /* ===== PSEUDOINSTRUCCIONES ===== */
        QStringLiteral("\\bnop\\b"), QStringLiteral("\\bli\\b"),
        QStringLiteral("\\bla\\b"),  QStringLiteral("\\bmv\\b"),
        QStringLiteral("\\bnot\\b"), QStringLiteral("\\bneg\\b"),
        QStringLiteral("\\bseqz\\b"), QStringLiteral("\\bsnez\\b"),
        QStringLiteral("\\bsltz\\b"), QStringLiteral("\\bsgtz\\b"),
        QStringLiteral("\\bbeqz\\b"), QStringLiteral("\\bbnez\\b"),
        QStringLiteral("\\bj\\b"), QStringLiteral("\\bjr\\b"),
        QStringLiteral("\\bret\\b"), QStringLiteral("\\bcall\\b"),
        QStringLiteral("\\btail\\b")
};
    const QString abiRegisterPatterns[] = {
        QStringLiteral("\\bzero\\b"),

        QStringLiteral("\\bra\\b"),

        QStringLiteral("\\bsp\\b"), QStringLiteral("\\bgp\\b"), QStringLiteral("\\btp\\b"),

        QStringLiteral("\\bt0\\b"), QStringLiteral("\\bt1\\b"), QStringLiteral("\\bt2\\b"),
        QStringLiteral("\\bt3\\b"), QStringLiteral("\\bt4\\b"), QStringLiteral("\\bt5\\b"),
        QStringLiteral("\\bt6\\b"),

        QStringLiteral("\\bs0\\b"), QStringLiteral("\\bs1\\b"),
        QStringLiteral("\\bs2\\b"), QStringLiteral("\\bs3\\b"),
        QStringLiteral("\\bs4\\b"), QStringLiteral("\\bs5\\b"),
        QStringLiteral("\\bs6\\b"), QStringLiteral("\\bs7\\b"),
        QStringLiteral("\\bs8\\b"), QStringLiteral("\\bs9\\b"),
        QStringLiteral("\\bs10\\b"), QStringLiteral("\\bs11\\b"),

        QStringLiteral("\\ba0\\b"), QStringLiteral("\\ba1\\b"),
        QStringLiteral("\\ba2\\b"), QStringLiteral("\\ba3\\b"),
        QStringLiteral("\\ba4\\b"), QStringLiteral("\\ba5\\b"),
        QStringLiteral("\\ba6\\b"), QStringLiteral("\\ba7\\b")
};


    if(language == 0 /*assembly*/){
        // ===================== RULES FOR HIGHLIGHTING ASSEMBLY ======================

        registerNumFormat.setForeground(QColor(47, 108, 150).lighter());    // REGISTERS

        for (const QString &pattern : keywordPatternsAssembly) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        for (const QString &pattern : abiRegisterPatterns){
            rule.pattern = QRegularExpression(pattern);
            rule.format = registerNumFormat;
            highlightingRules.append(rule);
        }


        // REGEX FOR REGISTER NUMBER
        registerNumFormat.setForeground(QColor(47, 108, 150).lighter());
        rule.pattern = QRegularExpression(QStringLiteral("\\b(x([0-9]|[12][0-9]|3[01]))\\b"));  // this makes sure x0-x31 are highlighted, but x32 not
        rule.format = registerNumFormat;
        highlightingRules.append(rule);

        // REGEX FOR COMMENTS
        singleLineCommentFormat.setForeground(Qt::gray);
        rule.pattern = QRegularExpression(QStringLiteral("#.*$"));
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::gray);

        // REGEX FOR STRINGS
        quotationFormat.setForeground(QColor(87, 175, 106));
        rule.pattern = QRegularExpression(QStringLiteral("(\".*\")|(<[^ >]+>)"));
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        // REGEX FOR VALUES (FALSE, TRUE, 12, +1, -2, 1.2, 1e10...)
        valuesFormat.setForeground(QColor(255, 158, 100));
        rule.pattern = QRegularExpression(QStringLiteral("\\b(true|false|TRUE|FALSE|[+-]?\\d+(\\.\\d+)?([eE][+-]?\\d+)?)\\b"));
        rule.format = valuesFormat;
        highlightingRules.append(rule);

        // REGEX FOR .global...
        directivesFormat.setFontWeight(QFont::Bold);
        directivesFormat.setForeground(QColor(160, 76, 89));
        rule.pattern = QRegularExpression(QStringLiteral("\\.(text|data|rodata|bss|globl|global|align|word|byte|half|ascii|asciz|section|equ|set)\\b"));
        rule.format = directivesFormat;
        highlightingRules.append(rule);

        // REGEX FOR LABELS (main:, _loop:...)
        labelsFormat.setFontWeight(QFont::Bold);
        labelsFormat.setForeground(QColor(35, 172, 184));
        rule.pattern = QRegularExpression(QStringLiteral("^\\s*[A-Za-z_][A-Za-z0-9_]*:"));
        rule.format = labelsFormat;
        highlightingRules.append(rule);


        commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
        commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

        // ==================================================================

    } else {
        for (const QString &pattern : keywordPatternsC) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        // ===================== RULES FOR HIGHLIGHTING C ======================

        // REGEX FOR COMMENTS
        singleLineCommentFormat.setForeground(Qt::gray);
        rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::gray);

        // REGEX FOR STRINGS
        quotationFormat.setForeground(QColor(87, 175, 106));
        rule.pattern = QRegularExpression(QStringLiteral("(\".*\")|(<[^ >]+>)"));
        rule.format = quotationFormat;
        highlightingRules.append(rule);


        // REGEX FOR VARIABLE FUNCTIONS
        //functionFormat.setFontItalic(true);
        functionFormat.setForeground(QColor(122, 162, 247));
        rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
        rule.format = functionFormat;
        highlightingRules.append(rule);


        // REGEX FOR VALUES (FALSE, TRUE, 12, +1, -2, 1.2, 1e10...)
        valuesFormat.setForeground(QColor(255, 158, 100));
        rule.pattern = QRegularExpression(QStringLiteral("\\b(true|false|TRUE|FALSE|[+-]?\\d+(\\.\\d+)?([eE][+-]?\\d+)?)\\b"));
        rule.format = valuesFormat;
        highlightingRules.append(rule);

        // REGEX FOR #
        hashtagFormat.setForeground(QColor(137, 221, 255));
        rule.pattern = QRegularExpression(QStringLiteral("#(?=(include|define)\\b)"));
        rule.format = hashtagFormat;
        highlightingRules.append(rule);



        commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
        commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

        // ==================================================================
    }


}


void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
