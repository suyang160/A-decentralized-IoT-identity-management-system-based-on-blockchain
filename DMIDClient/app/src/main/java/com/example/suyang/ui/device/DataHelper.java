package com.example.suyang.ui.device;

public class DataHelper {

    public static byte[] hexaStrToByteArray(final String hexaInput) {
        String hexa = hexaInput;

        if (hexa.length() == 1) {
            hexa = "0".concat(hexa);
        }

        final int resultLen = (hexa.length() + 1) / 2;
        byte[] result = new byte[resultLen];

        for (int i = 0; i < resultLen; i++) {
            result[i] = (byte) Integer.parseInt(hexa.substring(2 * i, 2 * i + 2), 16);
        }

        return result;
    }

    public static String byteArrayToHexaStr(final byte[] input) {
        if (input == null) {
            return "";
        }

        final int dataLen = input.length;
        final StringBuffer buff = new StringBuffer(dataLen * 2);

        for (int i = 0; i < dataLen; i++) {
            buff.append(convertDigit(input[i] >> 4));
            buff.append(convertDigit(input[i] & 15));
        }

        return buff.toString().toUpperCase();
    }
    public static String byteArrayToHexaStr(final byte[] input,int length) {
        if (input == null) {
            return "";
        }

        final int dataLen = length;
        final StringBuffer buff = new StringBuffer(dataLen * 2);

        for (int i = 0; i < dataLen; i++) {
            buff.append(convertDigit(input[i] >> 4));
            buff.append(convertDigit(input[i] & 15));
        }

        return buff.toString().toUpperCase();
    }

    private static char convertDigit(final int value) {
        int convertedValue = value;
        convertedValue &= 15;

        if (convertedValue >= 10) {
            return (char) (convertedValue - 10 + 'a');
        } else {
            return (char) (convertedValue + '0');
        }
    }

}
