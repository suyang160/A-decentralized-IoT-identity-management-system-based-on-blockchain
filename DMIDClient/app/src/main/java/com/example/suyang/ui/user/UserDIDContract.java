package com.example.suyang.ui.user;

import org.web3j.abi.TypeReference;
import org.web3j.abi.datatypes.Address;
import org.web3j.abi.datatypes.Bool;
import org.web3j.abi.datatypes.Function;
import org.web3j.abi.datatypes.Type;
import org.web3j.abi.datatypes.Utf8String;
import org.web3j.abi.datatypes.generated.Uint256;
import org.web3j.crypto.Credentials;
import org.web3j.protocol.Web3j;
import org.web3j.protocol.core.RemoteCall;
import org.web3j.protocol.core.methods.response.TransactionReceipt;
import org.web3j.tx.Contract;
import org.web3j.tx.TransactionManager;
import org.web3j.tx.gas.ContractGasProvider;

import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collections;

/**
 * <p>Auto generated code.
 * <p><strong>Do not modify!</strong>
 * <p>Please use the <a href="https://docs.web3j.io/command_line.html">web3j command line tools</a>,
 * or the org.web3j.codegen.SolidityFunctionWrapperGenerator in the
 * <a href="https://github.com/web3j/web3j/tree/master/codegen">codegen module</a> to update.
 *
 * <p>Generated with web3j version 4.2.0.
 */
public class UserDIDContract extends Contract {
    private static final String BINARY = "6080604052600c805460ff1916905534801561001a57600080fd5b5060008054600160a060020a0319163317815560068190556008819055600a819055600455610cff8061004e6000396000f3006080604052600436106100b95763ffffffff7c010000000000000000000000000000000000000000000000000000000060003504166326f5208981146100be578063391efdba1461014b578063534b3f681461019757806355a0bd5b146101f057806357b26c8b14610249578063700168671461025e5780637794fe2b146102765780638da5cb5b146102cf578063a61e199514610300578063ad97257314610327578063cdf0b5f914610394578063e1989cf2146103bb575b600080fd5b3480156100ca57600080fd5b506100d6600435610414565b6040805160208082528351818301528351919283929083019185019080838360005b838110156101105781810151838201526020016100f8565b50505050905090810190601f16801561013d5780820380516001836020036101000a031916815260200191505b509250505060405180910390f35b34801561015757600080fd5b50610195600160a060020a036004351660ff602435811690604435906064359060843581169060a43581169060c4359060e4359061010435166104c9565b005b3480156101a357600080fd5b506040805160206004803580820135601f81018490048402850184019095528484526101959436949293602493928401919081908401838280828437509497506106a59650505050505050565b3480156101fc57600080fd5b506040805160206004803580820135601f810184900484028501840190955284845261019594369492936024939284019190819084018382808284375094975061077a9650505050505050565b34801561025557600080fd5b506100d661082a565b34801561026a57600080fd5b506100d66004356108c1565b34801561028257600080fd5b506040805160206004803580820135601f81018490048402850184019095528484526101959436949293602493928401919081908401838280828437509497506108db9650505050505050565b3480156102db57600080fd5b506102e461098a565b60408051600160a060020a039092168252519081900360200190f35b34801561030c57600080fd5b50610315610999565b60408051918252519081900360200190f35b34801561033357600080fd5b506040805160206004803580820135601f810184900484028501840190955284845261038094369492936024939284019190819084018382808284375094975061099f9650505050505050565b604080519115158252519081900360200190f35b3480156103a057600080fd5b50610195600160a060020a036004351660ff60243516610ab2565b3480156103c757600080fd5b506040805160206004803580820135601f8101849004840285018401909552848452610195943694929360249392840191908190840183828082843750949750610b8c9650505050505050565b60606006548210156104c457600580548390811061042e57fe5b600091825260209182902001805460408051601f60026000196101006001871615020190941693909304928301859004850281018501909152818152928301828280156104bc5780601f10610491576101008083540402835291602001916104bc565b820191906000526020600020905b81548152906001019060200180831161049f57829003601f168201915b505050505090505b919050565b600160ff8616600381106104d957fe5b0160009054906101000a9004600160a060020a0316600160a060020a031660016004548b600160a060020a0316016001028a8a8a604051600081526020016040526040518085600019166000191681526020018460ff1660ff1681526020018360001916600019168152602001826000191660001916815260200194505050505060206040516020810390808403906000865af115801561057e573d6000803e3d6000fd5b50505060206040510351600160a060020a03161480156106635750600160ff8216600381106105a957fe5b0160009054906101000a9004600160a060020a0316600160a060020a031660016004548b600160a060020a031601600102868686604051600081526020016040526040518085600019166000191681526020018460ff1660ff1681526020018360001916600019168152602001826000191660001916815260200194505050505060206040516020810390808403906000865af115801561064e573d6000803e3d6000fd5b50505060206040510351600160a060020a0316145b15610691576000805473ffffffffffffffffffffffffffffffffffffffff1916600160a060020a038b161790555b505060048054600101905550505050505050565b600054600160a060020a0316331480156106c25750600c5460ff16155b151561073e576040805160e560020a62461bcd02815260206004820152602660248201527f4f6e6c79206f776e65722063616e206d6f646966792056435265766f6361746960448201527f6f6e4c6973740000000000000000000000000000000000000000000000000000606482015290519081900360840190fd5b80600760085481548110151561075057fe5b90600052602060002001908051906020019061076d929190610c3b565b5050600880546001019055565b600054600160a060020a0316331480156107975750600c5460ff16155b1515610813576040805160e560020a62461bcd02815260206004820152602260248201527f4f6e6c79206f776e65722063616e206d6f646966792064696420646f63756d6560448201527f6e74000000000000000000000000000000000000000000000000000000000000606482015290519081900360840190fd5b805161082690600b906020840190610c3b565b5050565b600b8054604080516020601f60026000196101006001881615020190951694909404938401819004810282018101909252828152606093909290918301828280156108b65780601f1061088b576101008083540402835291602001916108b6565b820191906000526020600020905b81548152906001019060200180831161089957829003601f168201915b505050505090505b90565b6060600a548210156104c457600980548390811061042e57fe5b600054600160a060020a0316331480156108f85750600c5460ff16155b151561094e576040805160e560020a62461bcd02815260206004820152601860248201527f4f6e6c79206f776e65722063616e206d6f646966792056430000000000000000604482015290519081900360640190fd5b806009600a5481548110151561096057fe5b90600052602060002001908051906020019061097d929190610c3b565b5050600a80546001019055565b600054600160a060020a031681565b60045490565b6000805b600854811015610aa757826040518082805190602001908083835b602083106109dd5780518252601f1990920191602091820191016109be565b5181516020939093036101000a600019018019909116921691909117905260405192018290039091206007805491945092508491508110610a1a57fe5b906000526020600020016040518082805460018160011615610100020316600290048015610a7f5780601f10610a5d576101008083540402835291820191610a7f565b820191906000526020600020905b815481529060010190602001808311610a6b575b50509150506040518091039020600019161415610a9f5760019150610aac565b6001016109a3565b600091505b50919050565b600054600160a060020a031633148015610acf5750600c5460ff16155b1515610b4b576040805160e560020a62461bcd02815260206004820152602a60248201527f4f6e6c79206f776e65722063616e206d6f6469667920736f6369616c2072656360448201527f6f76657279206c69737400000000000000000000000000000000000000000000606482015290519081900360840190fd5b81600160ff831660038110610b5c57fe5b01805473ffffffffffffffffffffffffffffffffffffffff1916600160a060020a03929092169190911790555050565b600054600160a060020a031633148015610ba95750600c5460ff16155b1515610bff576040805160e560020a62461bcd02815260206004820181905260248201527f4f6e6c79206f776e65722063616e206d6f64696679207472757374206c697374604482015290519081900360640190fd5b806005600654815481101515610c1157fe5b906000526020600020019080519060200190610c2e929190610c3b565b5050600680546001019055565b828054600181600116156101000203166002900490600052602060002090601f016020900481019282601f10610c7c57805160ff1916838001178555610ca9565b82800160010185558215610ca9579182015b82811115610ca9578251825591602001919060010190610c8e565b50610cb5929150610cb9565b5090565b6108be91905b80821115610cb55760008155600101610cbf5600a165627a7a72305820cc37c06c74b6579513b163f65d14d9ea4cda4d1279004c3778c199af54a34f830029";

    public static final String FUNC_ACQUIRETRUSTLIST = "acquireTrustList";

    public static final String FUNC_OWNERCHANGE = "OwnerChange";

    public static final String FUNC_ADDVCREVOCATIONLIST = "AddVCRevocationList";

    public static final String FUNC_ADDDIDDOCUMENT = "AddDIDDocument";

    public static final String FUNC_ACQUIREDIDDOCUMENT = "AcquireDIDDocument";

    public static final String FUNC_ACQUIREVC = "acquireVC";

    public static final String FUNC_ADDVC = "AddVC";

    public static final String FUNC_OWNER = "owner";

    public static final String FUNC_QUERYSOCIALRECOVERYACCOUNT = "QuerySocialRecoveryAccount";

    public static final String FUNC_ISINREVOCATIONLIST = "isInRevocationList";

    public static final String FUNC_ADDSOCIALRECOVERYLIST = "AddSocialRecoveryList";

    public static final String FUNC_ADDTRUSTLIST = "AddTrustList";

    @Deprecated
    protected UserDIDContract(String contractAddress, Web3j web3j, Credentials credentials, BigInteger gasPrice, BigInteger gasLimit) {
        super(BINARY, contractAddress, web3j, credentials, gasPrice, gasLimit);
    }

    protected UserDIDContract(String contractAddress, Web3j web3j, Credentials credentials, ContractGasProvider contractGasProvider) {
        super(BINARY, contractAddress, web3j, credentials, contractGasProvider);
    }

    @Deprecated
    protected UserDIDContract(String contractAddress, Web3j web3j, TransactionManager transactionManager, BigInteger gasPrice, BigInteger gasLimit) {
        super(BINARY, contractAddress, web3j, transactionManager, gasPrice, gasLimit);
    }

    protected UserDIDContract(String contractAddress, Web3j web3j, TransactionManager transactionManager, ContractGasProvider contractGasProvider) {
        super(BINARY, contractAddress, web3j, transactionManager, contractGasProvider);
    }

    public RemoteCall<String> acquireTrustList(BigInteger index) {
        final Function function = new Function(FUNC_ACQUIRETRUSTLIST,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.generated.Uint256(index)),
                Arrays.<TypeReference<?>>asList(new TypeReference<Utf8String>() {}));
        return executeRemoteCallSingleValueReturn(function, String.class);
    }

    public RemoteCall<TransactionReceipt> OwnerChange(String newowner, BigInteger v1, byte[] r1, byte[] s1, BigInteger index1, BigInteger v2, byte[] r2, byte[] s2, BigInteger index2) {
        final Function function = new Function(
                FUNC_OWNERCHANGE,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Address(newowner),
                        new org.web3j.abi.datatypes.generated.Uint8(v1),
                        new org.web3j.abi.datatypes.generated.Bytes32(r1),
                        new org.web3j.abi.datatypes.generated.Bytes32(s1),
                        new org.web3j.abi.datatypes.generated.Uint8(index1),
                        new org.web3j.abi.datatypes.generated.Uint8(v2),
                        new org.web3j.abi.datatypes.generated.Bytes32(r2),
                        new org.web3j.abi.datatypes.generated.Bytes32(s2),
                        new org.web3j.abi.datatypes.generated.Uint8(index2)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    public RemoteCall<TransactionReceipt> AddVCRevocationList(String _VCRev) {
        final Function function = new Function(
                FUNC_ADDVCREVOCATIONLIST,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Utf8String(_VCRev)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    public RemoteCall<TransactionReceipt> AddDIDDocument(String _didDocument) {
        final Function function = new Function(
                FUNC_ADDDIDDOCUMENT,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Utf8String(_didDocument)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    public RemoteCall<String> AcquireDIDDocument() {
        final Function function = new Function(FUNC_ACQUIREDIDDOCUMENT,
                Arrays.<Type>asList(),
                Arrays.<TypeReference<?>>asList(new TypeReference<Utf8String>() {}));
        return executeRemoteCallSingleValueReturn(function, String.class);
    }

    public RemoteCall<String> acquireVC(BigInteger index) {
        final Function function = new Function(FUNC_ACQUIREVC,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.generated.Uint256(index)),
                Arrays.<TypeReference<?>>asList(new TypeReference<Utf8String>() {}));
        return executeRemoteCallSingleValueReturn(function, String.class);
    }

    public RemoteCall<TransactionReceipt> AddVC(String _VC) {
        final Function function = new Function(
                FUNC_ADDVC,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Utf8String(_VC)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    public RemoteCall<String> owner() {
        final Function function = new Function(FUNC_OWNER,
                Arrays.<Type>asList(),
                Arrays.<TypeReference<?>>asList(new TypeReference<Address>() {}));
        return executeRemoteCallSingleValueReturn(function, String.class);
    }

    public RemoteCall<BigInteger> QuerySocialRecoveryAccount() {
        final Function function = new Function(FUNC_QUERYSOCIALRECOVERYACCOUNT,
                Arrays.<Type>asList(),
                Arrays.<TypeReference<?>>asList(new TypeReference<Uint256>() {}));
        return executeRemoteCallSingleValueReturn(function, BigInteger.class);
    }

    public RemoteCall<Boolean> isInRevocationList(String _vcHash) {
        final Function function = new Function(FUNC_ISINREVOCATIONLIST,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Utf8String(_vcHash)),
                Arrays.<TypeReference<?>>asList(new TypeReference<Bool>() {}));
        return executeRemoteCallSingleValueReturn(function, Boolean.class);
    }

    public RemoteCall<TransactionReceipt> AddSocialRecoveryList(String socialDid, BigInteger index) {
        final Function function = new Function(
                FUNC_ADDSOCIALRECOVERYLIST,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Address(socialDid),
                        new org.web3j.abi.datatypes.generated.Uint8(index)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    public RemoteCall<TransactionReceipt> AddTrustList(String _trustDid) {
        final Function function = new Function(
                FUNC_ADDTRUSTLIST,
                Arrays.<Type>asList(new org.web3j.abi.datatypes.Utf8String(_trustDid)),
                Collections.<TypeReference<?>>emptyList());
        return executeRemoteCallTransaction(function);
    }

    @Deprecated
    public static UserDIDContract load(String contractAddress, Web3j web3j, Credentials credentials, BigInteger gasPrice, BigInteger gasLimit) {
        return new UserDIDContract(contractAddress, web3j, credentials, gasPrice, gasLimit);
    }

    @Deprecated
    public static UserDIDContract load(String contractAddress, Web3j web3j, TransactionManager transactionManager, BigInteger gasPrice, BigInteger gasLimit) {
        return new UserDIDContract(contractAddress, web3j, transactionManager, gasPrice, gasLimit);
    }

    public static UserDIDContract load(String contractAddress, Web3j web3j, Credentials credentials, ContractGasProvider contractGasProvider) {
        return new UserDIDContract(contractAddress, web3j, credentials, contractGasProvider);
    }

    public static UserDIDContract load(String contractAddress, Web3j web3j, TransactionManager transactionManager, ContractGasProvider contractGasProvider) {
        return new UserDIDContract(contractAddress, web3j, transactionManager, contractGasProvider);
    }

    public static RemoteCall<UserDIDContract> deploy(Web3j web3j, Credentials credentials, ContractGasProvider contractGasProvider) {
        return deployRemoteCall(UserDIDContract.class, web3j, credentials, contractGasProvider, BINARY, "");
    }

    public static RemoteCall<UserDIDContract> deploy(Web3j web3j, TransactionManager transactionManager, ContractGasProvider contractGasProvider) {
        return deployRemoteCall(UserDIDContract.class, web3j, transactionManager, contractGasProvider, BINARY, "");
    }

    @Deprecated
    public static RemoteCall<UserDIDContract> deploy(Web3j web3j, Credentials credentials, BigInteger gasPrice, BigInteger gasLimit) {
        return deployRemoteCall(UserDIDContract.class, web3j, credentials, gasPrice, gasLimit, BINARY, "");
    }

    @Deprecated
    public static RemoteCall<UserDIDContract> deploy(Web3j web3j, TransactionManager transactionManager, BigInteger gasPrice, BigInteger gasLimit) {
        return deployRemoteCall(UserDIDContract.class, web3j, transactionManager, gasPrice, gasLimit, BINARY, "");
    }
}
