#include "testrunner.h"
#include <QtTest>

#include "test/testutiltest.h"
#include "test/s3test.h"
#include "test/buckettest.h"

TestRunner::TestRunner(int argc, char **argv, QObject *parent)
    : QObject(parent), _argc(argc), _argv(argv) {}

TestRunner::~TestRunner() {}

void TestRunner::run()
{
    int status = 0;
    auto ASSERT_TEST = [&status, this](QObject* obj) {
       status |= QTest::qExec(obj, this->_argc, this->_argv);
       delete obj;
    };

    ASSERT_TEST(new TestUtilTest());
    ASSERT_TEST(new S3Test());
    ASSERT_TEST(new BucketTest());

    QCoreApplication::instance()->exit(status);
}
