package io.agora.model;

import java.util.List;

public class MessageListBean {
    private String accountOther;
    private List<MessageBean> messageBeanList;

    public MessageListBean(String account, List<MessageBean> messageBeanList) {
        this.accountOther = account;
        this.messageBeanList = messageBeanList;
    }

    public String getAccountOther() {
        return accountOther;
    }

    public void setAccountOther(String accountOther) {
        this.accountOther = accountOther;
    }

    public List<MessageBean> getMessageBeanList() {
        return messageBeanList;
    }

    public void setMessageBeanList(List<MessageBean> messageBeanList) {
        this.messageBeanList = messageBeanList;
    }
}
