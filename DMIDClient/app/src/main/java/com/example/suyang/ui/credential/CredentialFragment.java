package com.example.suyang.ui.credential;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProviders;

import com.example.suyang.R;

public class CredentialFragment extends Fragment {

    private CredentialViewModel credentialViewModel;
    private TextView credentialInfoView;
    private Button read_credential_hold;
    private Button read_cerdential_issue;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        credentialViewModel =
                ViewModelProviders.of(this).get(CredentialViewModel.class);
        View root = inflater.inflate(R.layout.fragment_credential, container, false);
        credentialInfoView = root.findViewById(R.id.credential_Textview);
        credentialInfoView.setMovementMethod(ScrollingMovementMethod
                .getInstance());

        read_credential_hold = (Button) root.findViewById(R.id.read_credential_hold);

        read_credential_hold.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
//                credentialInfoView.append("用户持有证书：0\r\n");
            }
        });

        read_cerdential_issue = (Button) root.findViewById(R.id.read_credential_issue);

        read_cerdential_issue.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
//                credentialInfoView.append("用户颁发证书：1\r\n");
//                credentialInfoView.append("证书ID号：\"http://example.edu/credentials/1\"\r\n");
            }
        });
        
        return root;
    }
}