package com.example.suyang.ui.device;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Spannable;
import android.text.method.ScrollingMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProviders;

import com.example.suyang.R;
import com.example.suyang.bluetooth.BluetoothChatService;
import com.example.suyang.bluetooth.DeviceListActivity;

import java.security.SecureRandom;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import de.frank_durr.ecdh_curve25519.ECDHCurve25519;

public class DeviceFragment extends Fragment {

    // Debugging
    private static final String TAG = "BluetoothChat";
    private static final boolean D = true;
    private DeviceViewModel deviceViewModel;
    // 已连接设备的名称
    private String mConnectedDeviceName = null;
    // 输出流缓冲区
    private StringBuffer mOutStringBuffer;
    // 本地蓝牙适配器
    private BluetoothAdapter mBluetoothAdapter = null;
    // 用于通信的服务
    private com.example.suyang.bluetooth.BluetoothChatService mChatService = null;
    private Button search;
    private Button buildEncryptionChannel;
    private Button sendEncryptionMessage;
    private Button triggerCreateDMID;
    private Button issueCredential;
    private TextView mTitle;
    private TextView mConversationView;
    private boolean outhex = false;
    // 保存用数据缓存
    private String fmsg = "";

    // 来自BluetoothChatService Handler的消息类型
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    // 来自BluetoothChatService Handler的关键名
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    // Intent请求代码
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    //serial处理函数的变量
    private static byte[] functionCode=new byte[2];
    private static byte[]dataLength=new byte[2];
    private static byte[] dataReceive=new byte[100];
    private static byte[] dataServerPublicKey=new byte[32];
    private static int length=0;
    private static int functionCodeValue=0;
    enum current_state{
        none,
        head_state1,
        function_state1,
        function_state2,
        date_length1,
        data_length2,
        data_receive,
        end1,
        end2
    };
//    enum serialDataProcess_return{
//        data_receving, 0
//        data_error,    1
//        data_success   2
//    };
    static current_state  state= current_state.none;
    static int i=0;
    static int j=0;

    SecureRandom random = new SecureRandom();
    final byte[] alice_secret_key = ECDHCurve25519.generate_secret_key(random);
    final byte[] alice_public_key = ECDHCurve25519.generate_public_key(alice_secret_key);
    byte[] alice_shared_secret;
    private static final String cypherInstance = "AES/CBC/NoPadding";
    byte[] iv = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};




    static {
        try {
            System.loadLibrary("ecdhcurve25519");
            Log.i(TAG, "Loaded ecdhcurve25519 library.");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        deviceViewModel =
                ViewModelProviders.of(this).get(DeviceViewModel.class);
        View root = inflater.inflate(R.layout.fragment_device, container, false);
        search = (Button) root.findViewById(R.id.search);
        search.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                search();
            }
        });
        buildEncryptionChannel = (Button) root.findViewById(R.id.buildEncryptionChannel);

        buildEncryptionChannel.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
               // String message = DataHelper.byteArrayToHexaStr(alice_public_key);
                appendColoredText(mConversationView, "Protocol：start bit 0x07 0x41 + function code + data length + data + end bit 0x0A 0x0D\r\n", Color.BLUE);
                appendColoredText(mConversationView, "Phase1：Establish an encrypted channel\r\n", Color.RED);
                mConversationView.append("Generate private key based on Curve25519:0x"+DataHelper.byteArrayToHexaStr(alice_secret_key).toLowerCase()+"\r\n");
                mConversationView.append("Generate public key based on Curve25519:0x"+DataHelper.byteArrayToHexaStr(alice_public_key).toLowerCase()+"\r\n");
                mConversationView.append("Send the public key to the device，waiting...\r\n");
                sendMessageToMCU(1,alice_public_key.length,alice_public_key);
                Log.d(TAG,DataHelper.byteArrayToHexaStr(alice_public_key));
                Log.d(TAG,alice_public_key.length+"");
               // sendMessage(message);
            }
        });

        sendEncryptionMessage = (Button) root.findViewById(R.id.sendEncryptionMessage);
        sendEncryptionMessage.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String textToEncrypt = "helloWorld!12345";//必须是一个十六字节bytes
                try {
                    byte[] encrypt_value = aesEncrypt(textToEncrypt.getBytes(), alice_shared_secret, iv);
                    appendColoredText(mConversationView, "Test the AES symmetric key\r\n", Color.RED);
                    mConversationView.append("Local plain text:"+textToEncrypt+"\r\n");
                    mConversationView.append("Generate cypher text based on AES-CBC:0x"+DataHelper.byteArrayToHexaStr(encrypt_value).toLowerCase()+"\r\n");
                    mConversationView.append("Send cypher text to the device,waiting...\r\n");
                    sendMessageToMCU(20,encrypt_value.length,encrypt_value);

                } catch (Exception e) {
                    e.printStackTrace();
                    mConversationView.append("error\r\n");
                }

            }
        });

        triggerCreateDMID = (Button) root.findViewById(R.id.createDMID);
        triggerCreateDMID.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                    byte[] a = {0x01};
                    appendColoredText(mConversationView, "Triger device to generate DMID\r\n", Color.RED);
                    sendMessageToMCU(30,1,a);
//                    mConversationView.append("接到到设备端发送的challenge，发送response\r\n");
//                    mConversationView.append("设备DMID创建成功:did:example:0xa16cdca7ddfff32d4e5961de994520b76829a379\r\n");
                } catch (Exception e) {
                    e.printStackTrace();
                    mConversationView.append("error\r\n");
                }
            }
        });

        issueCredential = (Button) root.findViewById(R.id.issueCredential);
        issueCredential.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
//                    appendColoredText(mConversationView, "颁发证书给设备\r\n", Color.RED);
//                    mConversationView.append("本地生成证书ID:\"http://example.edu/credentials/1\"\r\n");
//                    mConversationView.append("发送成功\r\n");
                } catch (Exception e) {
                    e.printStackTrace();
                    mConversationView.append("error\r\n");
                }
            }
        });


        mConversationView = root.findViewById(R.id.messageReceiveTextView);
        mConversationView.setMovementMethod(ScrollingMovementMethod
                .getInstance());
        // 设置custom title
        mTitle = (TextView) root.findViewById(R.id.title_left_text);

//        userViewModel.getText().observe(this, new Observer<String>() {
//            @Override
//            public void onChanged(@Nullable String s) {
//           mTitle.setText(s);
//            }
//      });
        return root;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 获取本地蓝牙适配器
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // 如果没有蓝牙适配器，则不支持
        if (mBluetoothAdapter == null) {
            Toast.makeText(getActivity(), "蓝牙不可用", Toast.LENGTH_LONG).show();
            return;
        }

        //如果BT未打开，请求启用。
        // 然后在onActivityResult期间调用setupChat（）
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(
                    BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
            // 否则，设置聊天会话
        } else {
            if (mChatService == null)
                setupChat();
            else {
                try {
                    mChatService.wait(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

    }


    @Override
    public void onStop() {
        super.onStop();
        if (mChatService != null)
            mChatService.stop();
        if (D)
            Log.e(TAG, "--- ON DESTROY ---");

    }

    public void search() {
        Intent serverIntent = new Intent(getActivity(),
                DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }

    private void init(View root) {
        Log.d("UserFragment", "enter init");
        search = (Button) root.findViewById(R.id.search);
        Log.d("UserFragment", "leave init");
    }

    //初始化
    private void setupChat() {

        // 初始化BluetoothChatService以执行app_incon_bluetooth连接
        mChatService = new BluetoothChatService(getActivity(), mHandler);

        //初始化外发消息的缓冲区
        mOutStringBuffer = new StringBuffer("");
    }

    //重写发送函数，参数不同。
    private void sendMessage(String message) {
        // 确保已连接
        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(getActivity(), R.string.not_connected, Toast.LENGTH_SHORT)
                    .show();
            return;
        }
        // 检测是否有字符串发送
        if (message.length() > 0) {
            // 获取 字符串并告诉BluetoothChatService发送

            if (outhex == false) {
                byte[] send = message.getBytes();
                mChatService.write(send);//回调service
            }
            // 清空输出缓冲区
            mOutStringBuffer.setLength(0);
        } else {
            Toast.makeText(getActivity(), "发送内容不能为空", Toast.LENGTH_SHORT).show();
        }
    }

    // 该Handler从BluetoothChatService中获取信息
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_STATE_CHANGE:
                    if (D)
                        Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);


                    switch (msg.arg1) {
                        case BluetoothChatService.STATE_CONNECTED:
                            mTitle.setText(R.string.title_connected_to);
                            mTitle.append(mConnectedDeviceName);
                            mConversationView.setText(null);
                            break;

                        case BluetoothChatService.STATE_CONNECTING:
                            mTitle.setText(R.string.title_connecting);
                            break;

                        case BluetoothChatService.STATE_LISTEN:
                        case BluetoothChatService.STATE_NONE:
                            mTitle.setText(R.string.title_not_connected);
                            break;
                    }
                    break;

                case MESSAGE_WRITE:
                    byte[] writeBuf = (byte[]) msg.obj;
                    // 自动发送
                    // 发送计数
                    break;
                case MESSAGE_READ:

                    byte[] readBuf = (byte[]) msg.obj;
                    for(j=0;j< msg.arg1;j++) {
                        if (serialDataProcess(readBuf[j]) == 2) {
//                            mConversationView.append("functionCode is"+DataHelper.byteArrayToHexaStr(functionCode)+"\r\n");
//                            mConversationView.append("dataLength is"+DataHelper.byteArrayToHexaStr(dataLength)+"\r\n");
//                            mConversationView.append("dataReceive is"+DataHelper.byteArrayToHexaStr(dataReceive,length)+"\r\n");
                            mConversationView.append("FunctionCodeValue is:"+functionCodeValue+"\r\n");
                            if(functionCodeValue == 1)
                            {
                                for(j=0;j<32;j++) {
                                    dataServerPublicKey[j] = dataReceive[j];
                                }
                                mConversationView.append("Receive the public key sent from device:0x"+DataHelper.byteArrayToHexaStr(dataServerPublicKey).toLowerCase()+"\r\n");
                                alice_shared_secret = ECDHCurve25519.generate_shared_secret(
                                        alice_secret_key, dataServerPublicKey);
                                mConversationView.append("Generate symmetric key："+DataHelper.byteArrayToHexaStr(alice_shared_secret).toLowerCase()+"\r\n");
                            }
                            else if(functionCodeValue == 20)
                            {
                                byte[] cipherText=new byte[16];
                                for(j=0;j<16;j++) {
                                    cipherText[j] = dataReceive[j];
                                }
                                byte[] decrypt_value=new byte[16]; ;
                                mConversationView.append("Receives cipher text sent from the device side:0x"+DataHelper.byteArrayToHexaStr( cipherText).toLowerCase()+"\r\n");
                                try {
                                    decrypt_value = aesDecrypt(cipherText, alice_shared_secret, iv);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                                mConversationView.append("Decrypt the corresponding plaintext:"+new String(decrypt_value)+"\r\n");

                            }
                        }
                    }


                    //检错误码计算函数

//                    String readMessage = null;
//                    try {
//                        readMessage = new String(readBuf, 0, msg.arg1, "GBK");
//                    } catch (UnsupportedEncodingException e) {
//                        e.printStackTrace();
//                    }
//                    fmsg += readMessage;
//                    mConversationView.append(readMessage);
                    // 接收计数，更新UI
                    break;
                case MESSAGE_DEVICE_NAME:
                    // 保存已连接设备的名称
                    mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                    Toast.makeText(getActivity(),
                            "Connect to " + mConnectedDeviceName, Toast.LENGTH_SHORT)
                            .show();
                    break;
                case MESSAGE_TOAST:
                    Toast.makeText(getActivity(),
                            msg.getData().getString(TOAST), Toast.LENGTH_SHORT)
                            .show();
                    break;
            }
        }
    };

    //返回该Activity回调函数
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (D)
            Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) {

//search返回的
            case REQUEST_CONNECT_DEVICE:

                // DeviceListActivity返回时要连接的设备
                if (resultCode == Activity.RESULT_OK) {
                    // 获取设备的MAC地址
                    String address = data.getExtras().getString(
                            DeviceListActivity.EXTRA_DEVICE_ADDRESS);

                    // 获取BLuetoothDevice对象
                    BluetoothDevice device = mBluetoothAdapter
                            .getRemoteDevice(address);
                    // 尝试连接到设备
                    mChatService.connect(device);
                }
                break;
//start返回的（遇到蓝牙不可用退出）
            case REQUEST_ENABLE_BT:
                // 当启用蓝牙的请求返回时
                if (resultCode == Activity.RESULT_OK)
                {
                    //蓝牙已启用，因此设置聊天会话
                    setupChat();//初始化文本
                }
                else
                {
                    // 用户未启用蓝牙或发生错误
                    Log.d(TAG, "BT not enabled");

                    Toast.makeText(getActivity(), R.string.bt_not_enabled_leaving,
                            Toast.LENGTH_SHORT).show();
                }
        }
    }

    public static void appendColoredText(TextView tv, String text, int color) {
        int start = tv.getText().length();
        tv.append(text);
        int end = tv.getText().length();

        Spannable spannableText = (Spannable) tv.getText();
        spannableText.setSpan(new ForegroundColorSpan(color), start, end, 0);
    }

    public static byte[] aesEncrypt(byte[] textToEncrypt, byte[] secrectKey, byte[] iv) throws Exception {

        SecretKeySpec skeySpec = new SecretKeySpec(secrectKey, "AES");
        Cipher cipher = Cipher.getInstance(cypherInstance);
        cipher.init(Cipher.ENCRYPT_MODE, skeySpec, new IvParameterSpec(iv));
        byte[] encrypted = cipher.doFinal(textToEncrypt);
        return encrypted;
    }

    public static byte[] aesDecrypt(byte[] textToEncrypt,byte[] secrectKey, byte[] iv) throws Exception {

        SecretKeySpec skeySpec = new SecretKeySpec(secrectKey, "AES");
        Cipher cipher = Cipher.getInstance(cypherInstance);
        cipher.init(Cipher.DECRYPT_MODE, skeySpec, new IvParameterSpec(iv));
        byte[] encrypted = cipher.doFinal(textToEncrypt);
        return encrypted;
    }

    public static int serialDataProcess(byte receive_char) {
        byte data = receive_char;
        switch (state) {
            case none:
                if (data == 0x07) {
                    state = current_state.head_state1;
                    Log.i(TAG, "head_state1");
                    return 0;
                } else {
                    return 1;
                }
            case head_state1: {
                if (data == 0x41) {
                    state = current_state.function_state1;
                    Log.i(TAG, "function_state1");
                    return 0;
                } else {
                    state = current_state.none;
                    return 1;
                }
            }
            case function_state1: {
                functionCode[0] = data;
                state = current_state.function_state2;
                Log.i(TAG, "function_state2");
                return 0;
            }
            case function_state2: {
                functionCode[1] = data;
                state = current_state.date_length1;
                functionCodeValue = functionCode[0]*256+functionCode[1];
                return 0;
            }
            case date_length1: {
                dataLength[0] = data;
                state = current_state.data_length2;
                return 0;
            }
            case data_length2: {
                dataLength[1] = data;
                length = dataLength[0] * 256 + dataLength[1];
                state = current_state.data_receive;
                Log.i(TAG, "data_receive");
                return 0;
            }
            case data_receive: {
                dataReceive[i] = data;
                i++;
                if (i == length) {
                    i = 0;
                    state = current_state.end1;
                }
                return 0;
            }
            case end1: {
                if (data == 0x0a) {
                    state = current_state.end2;
                    return 0;
                } else {
                    state = current_state.none;
                    return 1;
                }
            }
            case end2: {
                if (data == 0x0D) {
                    state = current_state.none;
                    Log.i(TAG, "end2");
                    return 2;
                } else {
                    return 1;
                }
            }
            default:
                break;
        }
        return 1;
    }

    public  void sendMessageToMCU(int functioncode,int dataLength, byte[] data)
    {
        byte[]temp = new byte[dataLength+8];
        int i;
        temp[0]=0x07;
        temp[1]=0x41;
        temp[2]=(byte)(functioncode/256);
        temp[3]=(byte)(functioncode%256);
        temp[4]=(byte)(dataLength/256);
        temp[5]=(byte)(dataLength%256);
        for(i=0;i<dataLength;i++)
        {
            temp[6+i]=data[i];
        }
        temp[6+dataLength]=0x0A;
        temp[7+dataLength]=0x0D;

        mChatService.write(temp);

    }



}


