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
    void test_ranged_default();
    void test_ranged_deltas();
    void test_indirect_default();
    void test_indirect_deltas();
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

// 'default' means no step delta steps given
void TestIndexer::test_ranged_default()
{
    //Core::Indexer<int>* indexer = new Core::IndexerRanged<int>(0, 10, {0,1});
    Core::Indexer<int>* indexer = new Core::IndexerRanged<int>(0,10);
    QVector<int> results;
    while (indexer->available())
    {
        results.append( indexer->get() );
        indexer->next();
    }
    delete indexer;
    QCOMPARE(results, QVector({0,1,2,3,4,5,6,7,8,9}));

}

// deltas switch between advancing the index and not
void TestIndexer::test_ranged_deltas()
{
    Core::Indexer<int>* indexer = new Core::IndexerRanged<int>(0, 10, {0,1});
    QVector<int> results;
    while (indexer->available())
    {
        results.append( indexer->get() );
        indexer->next();
    }
    delete indexer;
    QCOMPARE(results, QVector({0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9}));

}

void TestIndexer::test_indirect_default()
{
    QVector<int> indirections({1,2,3,4,5});
    Core::Indexer<int>* indexer = new Core::IndexerIndirect<int>(indirections);
    QVector<int> results;
    while (indexer->available())
    {
        results.append( indexer->get() );
        indexer->next();
    }
    delete indexer;
    QCOMPARE(results, QVector({1,2,3,4,5}));
}

void TestIndexer::test_indirect_deltas()
{
    QVector<int> indirections({1,2,3,4,5});
    Core::Indexer<int>* indexer = new Core::IndexerIndirect<int>(indirections, {0,1});
    QVector<int> results;
    while (indexer->available())
    {
        results.append( indexer->get() );
        indexer->next();
    }
    delete indexer;
    QCOMPARE(results, QVector({1,1,2,2,3,3,4,4,5,5}));
}

void TestIndexer::test_case1()
{
    qDebug() << "Asdfasdf" << sizeof(QMatrix4x4);
}

QTEST_APPLESS_MAIN(TestIndexer)

#include "tst_testindexer.moc"
