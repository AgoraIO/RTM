package io.agora.model;

import io.agora.rtm.RtmMessage;

public class MessageBean {
    private String account;
    private RtmMessage message;
    private int background;
    private boolean beSelf;

    public MessageBean(String account, RtmMessage message, boolean beSelf) {
        this.account = account;
        this.message = message;
        this.beSelf = beSelf;
    }

    public String getAccount() {
        return account;
    }

    public void setAccount(String account) {
        this.account = account;
    }

    public RtmMessage getMessage() {
        return message;
    }

    public void setMessage(RtmMessage message) {
        this.message = message;
    }

    public boolean isBeSelf() {
        return beSelf;
    }

    public void setBeSelf(boolean beSelf) {
        this.beSelf = beSelf;
    }

    public int getBackground() {
        return background;
    }

    public void setBackground(int background) {
        this.background = background;
    }
}
