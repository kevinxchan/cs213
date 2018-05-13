package arch.sm213.machine.student;

import machine.AbstractMainMemory;
import org.junit.Before;
import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.*;

/**
 * Tests for MainMemory
 */
public class MainMemoryTest {
    private MainMemory testMainMemory;
    public static final int BYTE_CAPACITY = 8;

    private byte[] values = new byte[BYTE_CAPACITY];

    @Before
    public void runBefore() {
        this.testMainMemory = new MainMemory(BYTE_CAPACITY);
        // fill values with data
        this.values[0] = (byte) 0x10;
        this.values[1] = (byte) 0xff;
        this.values[2] = (byte) 0xa;
        this.values[3] = (byte) 0x44;
        this.values[4] = (byte) 0x84;
        this.values[5] = (byte) 0xcd;
        this.values[6] = (byte) 0xaa;
        this.values[7] = (byte) 0x18;
    }

    /**
     * test for address alignment.
     *
     * tests for:
     *  two cases where addresses are aligned (divisible by the length),
     *  one case where the address is not aligned (not divisible by length)
     */
    @Test
    public void testIsAccessAligned() {
        assertTrue(this.testMainMemory.isAccessAligned(BYTE_CAPACITY, 4));
        assertTrue(this.testMainMemory.isAccessAligned(6, 2));
        assertFalse(this.testMainMemory.isAccessAligned(6, 4));
    }

    /**
     * tests for method bytesToInteger.
     *
     * tests for:
     *  the lower and upper bounds (all bits 0 or all bits f),
     *  big endianness (ff 00 00 00 and 00 00 00 ff),
     *  one general case (01 02 03 04)
     */
    @Test
    public void testBytesToInteger() {
        assertEquals(-1, this.testMainMemory.bytesToInteger((byte) 0xff, (byte) 0xff, (byte) 0xff, (byte) 0xff));
        assertEquals(0, this.testMainMemory.bytesToInteger((byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00));
        assertEquals(-16777216, this.testMainMemory.bytesToInteger((byte) 0xff, (byte) 0x00, (byte) 0x00, (byte) 0x00));
        assertEquals(255, this.testMainMemory.bytesToInteger((byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xff));
        assertEquals(16909060, this.testMainMemory.bytesToInteger((byte) 0x01, (byte) 0x02, (byte) 0x03, (byte) 0x04));
    }

    /**
     * tests for method integerToBytes.
     *
     * tests for:
     *  case where all bytes should be set to ff (-1, upper bound)
     *  case where all bytes set to 00 (0, lower bound)
     *  case with a negative integer
     *  case with a positive integer
     *
     */
    @Test
    public void testIntegerToBytes() {
        byte[] retNegOne = this.testMainMemory.integerToBytes(-1);
        byte[] retZero = this.testMainMemory.integerToBytes(0);
        for(int i = 0; i < 4; i++){
            assertEquals((byte)0xff, retNegOne[i]);
            assertEquals((byte)0x00, retZero[i]);
        }
        byte[] retGeneral = this.testMainMemory.integerToBytes(-16645372);

        assertEquals((byte)0xff, retGeneral[0]);
        assertEquals((byte)0x02, retGeneral[1]);
        assertEquals((byte)0x03, retGeneral[2]);
        assertEquals((byte)0x04, retGeneral[3]);

        byte[] retGeneralPositive = this.testMainMemory.integerToBytes(8);

        assertEquals((byte)0x00, retGeneralPositive[0]);
        assertEquals((byte)0x00, retGeneralPositive[1]);
        assertEquals((byte)0x00, retGeneralPositive[2]);
        assertEquals((byte)0x08, retGeneralPositive[3]);
    }

    /**
     * test case for set method when exception should be thrown.
     *
     * tests for:
     *  if the byte array length to set to is not in range address to BYTE_CAPACITY
     */
    @Test (expected = AbstractMainMemory.InvalidAddressException.class)
    public void testSetException() throws AbstractMainMemory.InvalidAddressException {
        this.testMainMemory.set(BYTE_CAPACITY / 2, values);
    }

    /**
     * test case for get method when exception should be thrown.
     *
     * tests for:
     *  if an invalid memory address is trying to be accessed
     */
    @Test (expected = AbstractMainMemory.InvalidAddressException.class)
    public void testGetExceptionOutOfBounds() throws AbstractMainMemory.InvalidAddressException {
        this.testMainMemory.set(BYTE_CAPACITY - 1, new byte[]{(byte)0xff, (byte)0x11});
        this.testMainMemory.get(BYTE_CAPACITY - 2, 3);
    }

    /**
     * test case for get method when exception should be thrown.
     *
     * tests for:
     *  if starting address in memory allocation is invalid
     */
    @Test (expected = AbstractMainMemory.InvalidAddressException.class)
    public void testGetExceptionInvalidStartingAddress() throws AbstractMainMemory.InvalidAddressException {
        this.testMainMemory.get(BYTE_CAPACITY + 1, 1);
    }

    /**
     * test case to see if set and get methods behave as expected.
     * first test to see all bytes are set correctly, then change the last two bytes
     * to see if bytes are overwritten correctly
     */
    @Test
    public void testSetGetNoException() {
        try {
            this.testMainMemory.set(0, values);
            byte[] ret = this.testMainMemory.get(0, BYTE_CAPACITY);
            for (int i = 0; i < ret.length; i++)
                assertEquals(values[i], ret[i]);
            this.testMainMemory.set(BYTE_CAPACITY - 2, new byte[]{(byte)0x00, (byte)0x01});
            ret = this.testMainMemory.get(BYTE_CAPACITY - 2, 2);
            assertFalse(ret[0] == values[BYTE_CAPACITY - 2]);
            assertFalse(ret[1] == values[BYTE_CAPACITY - 1]);
        } catch (AbstractMainMemory.InvalidAddressException e) {
            fail("Unexpected exception");
        }
    }
}
