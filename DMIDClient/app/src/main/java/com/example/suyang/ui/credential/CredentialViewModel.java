package com.example.suyang.ui.credential;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class CredentialViewModel extends ViewModel {

    private MutableLiveData<String> mText;

    public CredentialViewModel() {
        mText = new MutableLiveData<>();
        mText.setValue("This is credential fragment");
    }

    public LiveData<String> getText() {
        return mText;
    }
}