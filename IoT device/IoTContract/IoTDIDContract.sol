pragma solidity ^0.5.1;
contract IoTDMIDContract{
    address public owner;
    string [100] trustList;
    uint256 trustList_count;
    string [1000] vcHold;
    uint256 vcHold_count;
    string didDocument;
    bool RevokedFlag = false;
    uint256 first_time_for_revoke;
    uint256 second_time_for_revoke;
    uint256 Lock_time = 86400;
    address emergency_contact;

    constructor () public{
        owner = msg.sender;
        trustList_count = 0;
        vcHold_count = 0;
    }


    function AddTrustList (string memory _trustDid) public
    {
        require(msg.sender == owner&&RevokedFlag==false,
        "Only owner can modify trust list"
        );
        trustList[trustList_count] = _trustDid;
        trustList_count++;
    }
    
    function acquireTrustList (uint256 index )public view returns(string memory result)
    {
        if(index < trustList_count)
        {
            return trustList[index];
        }
    }   

    function AddVC (string memory _VC) public
    {
        require(msg.sender == owner&&RevokedFlag==false,
        "Only owner can modify VC"
        );
        vcHold[vcHold_count] = _VC;
        vcHold_count++;
    }
    
    function acquireVC (uint256 index )public view returns(string memory result)
    {
        if(index < vcHold_count)
        {
            return vcHold[index];
        }
    } 

    function AddDMIDDocument (string memory _didDocument) public
    {
        require(msg.sender == owner&&RevokedFlag==false,
        "Only owner can modify did document"
        );
        didDocument=_didDocument;
    }
    
    function AcquireDMIDDocument() public view returns(string memory result)
    {
        return didDocument;
    }

    function SetEmergencyContact (address _EmergencyContact) public
    {
        require(msg.sender == owner&&RevokedFlag==false,
        "Only owner can set emergency contact"
        );
        emergency_contact = _EmergencyContact;
    }

    function OwnerChange (address newOwner) public
    {
        require(msg.sender == emergency_contact&&RevokedFlag==false,
        "Only owner can set emergency contact"
        );        
        owner = newOwner;
    }

    function DeleteDMID (uint8 index) public
    {
        require(msg.sender == owner&&RevokedFlag==false,
        "Only owner can deleteDMID"
        );
        if(index == 1)
        {
            first_time_for_revoke = block.timestamp;
        }
        if(index == 2)
        {
            second_time_for_revoke = block.timestamp;
            if(second_time_for_revoke- first_time_for_revoke > Lock_time)
            {
                RevokedFlag = true;
            }
        }
    }
}