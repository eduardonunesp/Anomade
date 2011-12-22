#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat hFormat;
    QTextCharFormat italicFormat;
    QTextCharFormat boldFormat;
    QTextCharFormat listFormat;
    QTextCharFormat quoteFormat;
    QTextCharFormat rulesFormat;
    QTextCharFormat linkFormat;
    QTextCharFormat codeFormat;
    QTextCharFormat imageFormat;
};

#endif
