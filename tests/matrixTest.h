/*
 * @author  Dieter J Kybelksties
 * @date 13-Oct-2019
 *
 */

/*
 * File:   matrixTest.h
 * Author: Dieter J Kybelksties
 *
 * Created on 13-Oct-2019, 14:28:53
 */

#ifndef MATRIXTEST_H
#define MATRIXTEST_H

#include <cppunit/extensions/HelperMacros.h>

class matrixTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(matrixTest);

    //    CPPUNIT_TEST(testMatrixConstruction);
    CPPUNIT_TEST(testMatrixOperations);

    CPPUNIT_TEST_SUITE_END();

public:
    matrixTest();
    virtual ~matrixTest();
    void setUp();
    void tearDown();

private:
    void testMatrixConstruction();
    void testMatrixOperations();
};

#endif /* MATRIXTEST_H */

