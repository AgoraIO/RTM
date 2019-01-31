package io.agora.model;

public class MessageBean {
    private String account;
    private String message;
    private int background;
    private boolean beSelf;

    public MessageBean(String account, String message, boolean beSelf) {
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

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
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
