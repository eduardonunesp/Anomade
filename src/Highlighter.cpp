#include "Highlighter.hpp"
#include <QtGui>

Highlighter::Highlighter(QTextDocument *parent)
: QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    hFormat.setForeground(QColor(0, 0, 150 ));
    hFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("#+( )?[A-Za-z]+.*(#+)?$");
    rule.format = hFormat;
    highlightingRules.append(rule);

    italicFormat.setForeground(QColor(150, 0, 0));
    rule.pattern = QRegExp("(\\*|_)[A-Za-z ]+(\\*|_)");
    rule.format = italicFormat;
    highlightingRules.append(rule);

    boldFormat.setForeground(QColor(150, 0, 0));
    rule.pattern = QRegExp("(\\*\\*|__)[A-Za-z ]+(\\*\\*|__)");
    rule.format = boldFormat;
    highlightingRules.append(rule);

    listFormat.setForeground(QColor(0, 150, 0));
    rule.pattern = QRegExp("^(-|\\+|\\*|[0-9]+\\.) [A-Za-z]+.*$");
    rule.format = listFormat;
    highlightingRules.append(rule);

    quoteFormat.setForeground(QColor(150, 150, 150));
    rule.pattern = QRegExp("^>.*$");
    rule.format = quoteFormat;
    highlightingRules.append(rule);

    rulesFormat.setForeground(QColor(0, 0, 150));
    rulesFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("^(\\s|\\s\\s|\\s\\s\\s)\\s(-|--|---)\\s.*$");
    rule.format = rulesFormat;
    highlightingRules.append(rule);

    linkFormat.setForeground(QColor(0, 150, 0));
    rule.pattern = QRegExp("\\[.*\\]\\(.*(\\s\\\".*\\\")?\\)");
    rule.format = linkFormat;
    highlightingRules.append(rule);

    codeFormat.setForeground(QColor(0, 0, 150));
    rule.pattern = QRegExp("`.*`");
    rule.format = codeFormat;
    highlightingRules.append(rule);

    imageFormat.setForeground(QColor(0, 150, 0));
    rule.pattern = QRegExp("!\\[.*\\]\\(.*(\\s\\\".*\\\")?\\)");
    rule.format = imageFormat;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);
}
