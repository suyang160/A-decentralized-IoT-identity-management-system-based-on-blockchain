package com.example.suyang.ui.user;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
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

import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.web3j.crypto.Bip32ECKeyPair;
import org.web3j.crypto.Credentials;
import org.web3j.crypto.ECKeyPair;
import org.web3j.crypto.Keys;
import org.web3j.crypto.MnemonicUtils;
import org.web3j.crypto.Wallet;
import org.web3j.crypto.WalletFile;
import org.web3j.protocol.Web3j;
import org.web3j.protocol.core.methods.response.TransactionReceipt;
import org.web3j.protocol.http.HttpService;
import org.web3j.tx.gas.DefaultGasProvider;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.math.BigInteger;
import java.security.Provider;
import java.security.Security;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import io.textile.ipfslite.BuildConfig;
import io.textile.ipfslite.Peer;


public class UserFragment extends Fragment {

    private UserViewModel userViewModel;
    private TextView userDIDInfoView;
    private Button button_test;
    private Button button_ipfs;
    private Button button_file_write;
    private Button button_file_read;


    private static byte[] textToEncrypt =  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
    private static byte[] secrectKey={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
    private static byte[] iv = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};

    private static final int pswdIterations = 10;
    private static final int keySize = 128;
    private static final String cypherInstance = "AES/CBC/NoPadding";
    private static final String secretKeyInstance = "PBKDF2WithHmacSHA1";
    private static final String plainText = "sampleText";
    private static final String AESSalt = "exampleSalt";
    private static final String initializationVector = "8119745113154120";
    private String password = "PASSWORD";
    private WalletFile wallet;
    private static final int NUM1 = 0x21;
    private static final int NUM2 = 0x22;
    private static final int NUM3 = 0x23;
    Peer litePeer;
    private static int fileCount = 0;

    private void initIPFS() {
        try {
            Context ctx = getActivity();
            final File filesDir = ctx.getFilesDir();
            final String path = new File(filesDir, "ipfslite").getAbsolutePath();
            litePeer = new Peer(path, BuildConfig.DEBUG);
            litePeer.start();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        userViewModel =
                ViewModelProviders.of(this).get(UserViewModel.class);
        View root = inflater.inflate(R.layout.fragment_user, container, false);
        userDIDInfoView = root.findViewById(R.id.test1);
        userDIDInfoView.setMovementMethod(ScrollingMovementMethod
                .getInstance());

//        button_file_write = (Button) root.findViewById(R.id.button_file_write);
//        button_file_write.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                fileCount++;
//                save(fileCount+"");
//                userDIDInfoView.append("Write:"+fileCount+""+"\r\n");
//
//            }
//        });
//
//        button_file_read= (Button) root.findViewById(R.id.button_file_read);
//        button_file_read.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                read();
//
//                JSONObject obj_DIDDocument= new JSONObject();
//                JSONArray array_publicKey =new JSONArray();
//                JSONObject object_publicKey =new JSONObject();
//                JSONArray array_authentication =new JSONArray();
//                JSONObject object_authentication =new JSONObject();
//                try{
//                    obj_DIDDocument.put("@context","https://www.w3.org/ns/did/v1");
//                    obj_DIDDocument.put("id","did:example:123456789abcdefghi");
//                    object_publicKey.put("id","did:example:123456789abcdefghi#keys-1");
//                    object_publicKey.put("type","Secp256k1VerificationKey2018");
//                    object_publicKey.put("controller","did:example:123456789abcdefghi");
//                    object_publicKey.put("publicKeyBase58", "H3C2AVvLMv6gmMNam3uVAjZpfkcJCwDwnZn6z3wXmqPV");
//                    array_publicKey.put(object_publicKey);
//                    obj_DIDDocument.put("publicKey",array_publicKey);
//                    array_authentication.put("#key-1");
//                    obj_DIDDocument.put("authentication",array_authentication);
//                    userDIDInfoView.append("\r\nobj_DIDDocument is"+obj_DIDDocument.toString());
//
//                } catch (JSONException e) {
//                    e.printStackTrace();
//                }
//
//            }
//        });
//
//        button_ipfs = (Button) root.findViewById(R.id.button_ipfs);
//        button_ipfs.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
////                try {
////                    String cid = litePeer.addFileSync("Hello World Suyang".getBytes());
////                    Log.d("UserFragment","Success: " + cid);
////                } catch (Exception e) {
////                    Log.e("UserFragment", "error : ", e);
////                }
//                initIPFS();
//                try {
//                    //byte[] file = litePeer.getFileSync("bafybeic35nent64fowmiohupnwnkfm2uxh6vpnyjlt3selcodjipfrokgi");
//                    byte[] file = litePeer.getFileSync("bafybeic2yi66degjatcxmd36i4jktyfwvap3sd67kfeeoh4btq3q3rpb4y");
//                    Log.d("UserFragment","Success byte is: " + new String(file, "UTF-8"));
//
//                } catch (Exception e) {
//                    Log.e("UserFragment", "error : ", e);
//                }
//
//            }
//        });


        button_test = (Button) root.findViewById(R.id.button_test);
        button_test.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

////                setupBouncyCastle();
////
////
////                try {
////                    wallet = createWallet();
////                } catch (Exception e) {
////                    System.out.println("BIG RIP");
////                }
//
//                Web3j web3 = Web3j.build(new HttpService("http://192.168.1.103:8545/"));  // defaults to http://localhost:8545/
////                Web3ClientVersion web3ClientVersion = null;
////                try {
////                    web3ClientVersion = web3.web3ClientVersion().sendAsync().get();
////                } catch (InterruptedException e) {
////                    e.printStackTrace();
////                } catch (ExecutionException e) {
////                    e.printStackTrace();
////                }
////                String clientVersion = web3ClientVersion.getWeb3ClientVersion();
////                userDIDInfoView.append("clientVersion is:"+clientVersion+"\r\n");
////
////                EthBlockNumber result = null;
////                try {
////                    result = web3.ethBlockNumber().sendAsync().get();
////                } catch (InterruptedException e) {
////                    e.printStackTrace();
////                } catch (ExecutionException e) {
////                    e.printStackTrace();
////                }
////                userDIDInfoView.append("Current BlockNumber is:"+result.getBlockNumber().toString()+"\r\n");
////
//                String mnemonic = "snap escape shadow school illness flip hollow label melt fetch noise install";
//
//                //m/44'/60'/0'/0 derivation path
//                int[] derivationPath = {44 | Bip32ECKeyPair.HARDENED_BIT, 60 | Bip32ECKeyPair.HARDENED_BIT, 0 | Bip32ECKeyPair.HARDENED_BIT, 0,0};
//
//                // Generate a BIP32 master keypair from the mnemonic phrase
//                Bip32ECKeyPair masterKeypair = Bip32ECKeyPair.generateKeyPair(MnemonicUtils.generateSeed(mnemonic, password));
//
//                // Derive the keypair using the derivation path
//                Bip32ECKeyPair  derivedKeyPair = Bip32ECKeyPair.deriveKeyPair(masterKeypair, derivationPath);
//
//                // Load the wallet for the derived keypair
//                Credentials credentials = Credentials.create(derivedKeyPair);
//
//                userDIDInfoView.append("privateKey is:"+ DataHelper.byteArrayToHexaStr(derivedKeyPair.getPrivateKeyBytes33())+"\r\n");
//                userDIDInfoView.append("publicKey is:"+derivedKeyPair.getPublicKeyPoint().toString()+"\r\n");
//                userDIDInfoView.append("Address is:"+credentials.getAddress()+"\r\n");
//
////                EthGetTransactionCount ethGetTransactionCount = null;
////                try {
////                    ethGetTransactionCount = web3.ethGetTransactionCount(
////                            credentials.getAddress(), DefaultBlockParameterName.LATEST).sendAsync().get();
////                } catch (InterruptedException e) {
////                    e.printStackTrace();
////                } catch (ExecutionException e) {
////                    e.printStackTrace();
////                }
////                BigInteger nonce = ethGetTransactionCount.getTransactionCount();
////                String recipientAddress = "0x5d361e4fb974e4503028908588aa3d2130baabad";
////                BigInteger value = Convert.toWei("1", Convert.Unit.ETHER).toBigInteger();
////                BigInteger gasLimit = BigInteger.valueOf(21000);
////                BigInteger gasPrice = Convert.toWei("1", Convert.Unit.GWEI).toBigInteger();
////                RawTransaction rawTransaction  = RawTransaction.createEtherTransaction(
////                        nonce, gasPrice, gasLimit, recipientAddress, value);
////                byte[] signedMessage = TransactionEncoder.signMessage(rawTransaction, credentials);
////                String hexValue = Numeric.toHexString(signedMessage);
////                try {
////                    EthSendTransaction ethSendTransaction = web3.ethSendRawTransaction(hexValue).sendAsync().get();
////                    // Get the transaction hash
////                } catch (InterruptedException e) {
////                    e.printStackTrace();
////                } catch (ExecutionException e) {
////                    e.printStackTrace();
////                }
//
//                try {
//                    Log.e("UserFragment", "point1");
//                    //SimpleStorage simpleStorage = SimpleStorage.deploy(web3,credentials,new DefaultGasProvider()).sendAsync().get() ;
//                    String address1 = "0x99d3381947993ce77093fd90ea55fb172bb85d15";
//                    SimpleStorage simpleStorage = SimpleStorage.load(address1,web3,credentials,new DefaultGasProvider());
//                    Log.e("UserFragment", "point2");
//                    String contractAddress = simpleStorage.getContractAddress();
//                    Log.d("contractAddress",  contractAddress);
//                    userDIDInfoView.append("Contract address is:"+contractAddress+"\r\n");
////                    BigInteger temp1 = BigInteger.valueOf(100);
////                    TransactionReceipt transactionReceipt = simpleStorage.set(temp1).sendAsync().get();
//                    BigInteger result = simpleStorage.get().sendAsync().get();
//                    userDIDInfoView.append("result is:"+result+"\r\n");
//
//                } catch (Exception e) {
//                    Log.e("UserFragment", "error : ", e);
//                }


                new MyThread().start();
           }
        });

        return root;
    }

    private Handler myHandle = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case NUM1:
                    Bundle bundle = msg.getData();
                    String mnemonic = bundle.getString("mnemonic");
                    String privateKey = bundle.getString("privateKey");
                    String publicKey = bundle.getString("publicKey");
                    String Address = bundle.getString("Address");
                    appendColoredText(userDIDInfoView, "1.Generate Key locally\r\n", Color.RED);
                    userDIDInfoView.append("Mnemonic word:"+ mnemonic+"\r\n");
                    userDIDInfoView.append("Private key:"+ privateKey+"\r\n");
                    userDIDInfoView.append("Public key:"+ publicKey+"\r\n");
                    userDIDInfoView.append("Address:"+ Address+"\r\n");
                    break;
                case NUM2:
                    if(msg.arg1==1) {
                        Bundle bundle2 = msg.getData();
                        appendColoredText(userDIDInfoView, "2.Deploy DID smart contract\r\n", Color.RED);
                        String contractAddress = bundle2.getString("contractAddress");
                        userDIDInfoView.append("Contract Address:" + contractAddress + "\r\n");
                        break;
                    }else if(msg.arg1==2){
                        Bundle bundle2 = msg.getData();
                        String userDidDocument = bundle2.getString("userDidDocument");
                        appendColoredText(userDIDInfoView, "3.Generate user's DID Document locally:\r\n", Color.RED);
                        userDIDInfoView.append(userDidDocument + "\r\n");
                        break;
                    }else if(msg.arg1==3){
                         Bundle bundle2 = msg.getData();
                         String cid = bundle2.getString("cid");
                         appendColoredText(userDIDInfoView, "4.Store user's DID Document to IPFS network:\r\n", Color.RED);
                         userDIDInfoView.append("User DID Document hash:" + cid + "\r\n");
                         break;
                    }

                case NUM3:
                    Bundle bundle3 = msg.getData();
                    String result = bundle3.getString("result");
                    String TransactionHash = bundle3.getString("TransactionHash");
                    String getBlockNumber = bundle3.getString("getBlockNumber");
                    //userDIDInfoView.append("result is:"+ result+"\r\n");
                    appendColoredText(userDIDInfoView, "5.Modify the Document property of user's DID contract:\r\n", Color.RED);
                    userDIDInfoView.append("TransactionHash is:"+ TransactionHash+"\r\n");
                    userDIDInfoView.append("getBlockNumberis:"+ getBlockNumber+"\r\n");
                    break;
                default:
                    break;
            }

        }
    };



    public class MyThread extends Thread {
        @Override
        public void run() {
            //Web3j web3 = Web3j.build(new HttpService("http://192.168.31.111:8545/"));
           Web3j web3 = Web3j.build(new HttpService("https://rinkeby.infura.io/v3/c44024b2f8414bbf8cd86990f3d74604"));

            String mnemonic = "snap escape shadow school illness flip hollow label melt fetch noise install";
            //m/44'/60'/0'/0 derivation path
            int[] derivationPath = {44 | Bip32ECKeyPair.HARDENED_BIT, 60 | Bip32ECKeyPair.HARDENED_BIT, 0 | Bip32ECKeyPair.HARDENED_BIT, 0,0};
            // Generate a BIP32 master keypair from the mnemonic phrase
            Bip32ECKeyPair masterKeypair = Bip32ECKeyPair.generateKeyPair(MnemonicUtils.generateSeed(mnemonic, password));
            // Derive the keypair using the derivation path
            Bip32ECKeyPair  derivedKeyPair = Bip32ECKeyPair.deriveKeyPair(masterKeypair, derivationPath);
            // Load the wallet for the derived keypair
            Credentials credentials = Credentials.create(derivedKeyPair);

            Message message = new Message();//传递数据
            message.what = NUM1;
            Bundle bundle = new Bundle();
            bundle.putString("mnemonic",mnemonic);
            String prv = new BigInteger(derivedKeyPair.getPrivateKey().toString(),10).toString(16);
            bundle.putString("privateKey",prv);
            //bundle.putString("publicKey",derivedKeyPair.getPublicKeyPoint().toString());

            String pub = new BigInteger(derivedKeyPair.getPublicKey().toString(),10).toString(16);
            //公钥64位
            bundle.putString("publicKey",pub);
            bundle.putString("Address",credentials.getAddress());
            message.setData(bundle);
            myHandle.sendMessage(message);

//                userDIDInfoView.append("privateKey is:"+ DataHelper.byteArrayToHexaStr(derivedKeyPair.getPrivateKeyBytes33())+"\r\n");
//                userDIDInfoView.append("publicKey is:"+derivedKeyPair.getPublicKeyPoint().toString()+"\r\n");
//                userDIDInfoView.append("Address is:"+credentials.getAddress()+"\r\n");

            try {
                Log.d("UserFragment", "point1");
                //SimpleStorage simpleStorage = SimpleStorage.deploy(web3,credentials,new DefaultGasProvider()).sendAsync().get();
                UserDIDContract userDIDContract = UserDIDContract.deploy(web3,credentials,new DefaultGasProvider()).sendAsync().get();
                Log.d("UserFragment", "point2");
                String contractAddress = userDIDContract.getContractAddress();
                Log.d("contractAddress",  contractAddress);

                Message message2 = new Message();//传递数据
                message2.what = NUM2;
                message2.arg1 = 1;
                Bundle bundle2 = new Bundle();
                bundle2.putString("contractAddress",contractAddress);
                message2.setData(bundle2);
                myHandle.sendMessage(message2);

                //生成didDocument
                String userDidDocument = generateDIDDocument("did:example:"+contractAddress,pub);
                Message message3 = new Message();
                message3.what = NUM2;
                message3.arg1 = 2;
                Bundle bundle3 = new Bundle();
                bundle3.putString("userDidDocument",userDidDocument);
                message3.setData(bundle3);
                myHandle.sendMessage(message3);

                //存储到IPFS网络
                initIPFS();
                String cid=null;
                try {
                    cid = litePeer.addFileSync(userDidDocument.getBytes());
                    Log.d("UserFragment","Success: " + cid);
                } catch (Exception e) {
                    Log.e("UserFragment", "error : ", e);
                    Toast.makeText(getActivity(), "IPFS异常", Toast.LENGTH_LONG).show();
                }
                Message message4 = new Message();
                message4.what = NUM2;
                message4.arg1 = 3;
                Bundle bundle4 = new Bundle();
                bundle4.putString("cid",cid);
                message4.setData(bundle4);
                myHandle.sendMessage(message4);


                TransactionReceipt transactionReceipt =  userDIDContract.AddDIDDocument(cid).sendAsync().get();
                String result =  userDIDContract.AcquireDIDDocument().send();
                Log.d("result is",result+"");

                Message message5 = new Message();//传递数据
                message5.what = NUM3;
                Bundle bundle5 = new Bundle();
                bundle5.putString("result",result+"");
                bundle5.putString("TransactionHash",transactionReceipt.getTransactionHash());
                bundle5.putString("getBlockNumber",transactionReceipt.getBlockNumber()+"");
                message5.setData(bundle5);
                myHandle.sendMessage(message5);

            } catch (Exception e) {
                Log.e("UserFragment", "error : ", e);
                Looper.prepare();
                Toast.makeText(getActivity(), "网络连接异常", Toast.LENGTH_LONG).show();
                Looper.loop();
            }

        }
    }

    public WalletFile createWallet() throws Exception {
        ECKeyPair keyPair = Keys.createEcKeyPair();
        return Wallet.createLight(password, keyPair);

    }


    private void setupBouncyCastle() {
        final Provider provider = Security.getProvider(BouncyCastleProvider.PROVIDER_NAME);
        if (provider == null) {
            // Web3j will set up the provider lazily when it's first used.
            return;
        }
        if (provider.getClass().equals(BouncyCastleProvider.class)) {
            // BC with same package name, shouldn't happen in real life.
            return;
        }
        // Android registers its own BC provider. As it might be outdated and might not include
        // all needed ciphers, we substitute it with a known BC bundled in the app.
        // Android's BC has its package rewritten to "com.android.org.bouncycastle" and because
        // of that it's possible to have another BC implementation loaded in VM.
        Security.removeProvider(BouncyCastleProvider.PROVIDER_NAME);
        Security.insertProviderAt(new BouncyCastleProvider(), 1);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //userDIDInfoView.append("hahahah");


    }

    public static void appendColoredText(TextView tv, String text, int color) {
        int start = tv.getText().length();
        tv.append(text);
        int end = tv.getText().length();

        Spannable spannableText = (Spannable) tv.getText();
        spannableText.setSpan(new ForegroundColorSpan(color), start, end, 0);
    }

    public byte[] encrypt(String content, String password) throws Exception {
        // 创建AES秘钥
        SecretKeySpec key = new SecretKeySpec(password.getBytes(), "AES/CBC/PKCS5PADDING");
        // 创建密码器
        Cipher cipher = Cipher.getInstance("AES");
        // 初始化加密器
        cipher.init(Cipher.ENCRYPT_MODE, key);
        // 加密
        return cipher.doFinal(content.getBytes("UTF-8"));
    }

    public static byte[] encrypt1(byte[] textToEncrypt) throws Exception {

        SecretKeySpec skeySpec = new SecretKeySpec(secrectKey, "AES");
        Cipher cipher = Cipher.getInstance(cypherInstance);
        cipher.init(Cipher.ENCRYPT_MODE, skeySpec, new IvParameterSpec(iv));
        byte[] encrypted = cipher.doFinal(textToEncrypt);
        return encrypted;
    }
    public static byte[] decrypt1(byte[] textToEncrypt) throws Exception {

        SecretKeySpec skeySpec = new SecretKeySpec(secrectKey, "AES");
        Cipher cipher = Cipher.getInstance(cypherInstance);
        cipher.init(Cipher.DECRYPT_MODE, skeySpec, new IvParameterSpec(iv));
        byte[] encrypted = cipher.doFinal(textToEncrypt);
        return encrypted;
    }

    private void save(String inputText ) {
        FileOutputStream fos = null;
        BufferedWriter writer = null;
        try {
            fos = getActivity().openFileOutput("data", Context.MODE_PRIVATE);
            writer = new BufferedWriter(new OutputStreamWriter(fos));
            writer.write(inputText);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            try {
                if (writer != null)
                    writer.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }


    private void read( ) {
        FileInputStream fis = null;
        BufferedReader reader = null;

        try {
            fis = getActivity().openFileInput("data");
            reader = new BufferedReader(new InputStreamReader(fis));
            String line = "";
            while ((line = reader.readLine()) != null) {
                userDIDInfoView.append("read is"+line);
            }
        } catch (IOException e) {
            Log.e("UserFragment", "error : ", e);
        } finally {
            try {
                if (reader != null)
                    reader.close();
            } catch (IOException e) {
                Log.e("UserFragment", "error : ", e);
            }
        }
    }

    private String generateDIDDocument(String did_user,String publicKey)
    {
        JSONObject obj_DIDDocument= new JSONObject();
        JSONArray array_publicKey =new JSONArray();
        JSONObject object_publicKey =new JSONObject();
        JSONArray array_authentication =new JSONArray();
        JSONObject object_authentication =new JSONObject();
        try{
            obj_DIDDocument.put("@context","https://www.w3.org/ns/did/v1");
            obj_DIDDocument.put("id",did_user);
            object_publicKey.put("id",did_user+"#keys-1");
            object_publicKey.put("type","Secp256k1VerificationKey2020");
            object_publicKey.put("controller",did_user);
            object_publicKey.put("publicKey", publicKey);
            array_publicKey.put(object_publicKey);
            obj_DIDDocument.put("publicKey",array_publicKey);
            array_authentication.put("#key-1");
            obj_DIDDocument.put("authentication",array_authentication);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return obj_DIDDocument.toString();
    }

}