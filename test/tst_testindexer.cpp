#include <QtTest>

// add necessary includes here

#include "../app/mesh.h"

class TestIndexer : public QObject
{
    Q_OBJECT

public:
    TestIndexer();
    ~TestIndexer();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TestIndexer::TestIndexer()
{

}

TestIndexer::~TestIndexer()
{

}

void TestIndexer::initTestCase()
{

}

void TestIndexer::cleanupTestCase()
{

}

void TestIndexer::test_case1()
{

}

QTEST_APPLESS_MAIN(TestIndexer)

#include "tst_testindexer.moc"
