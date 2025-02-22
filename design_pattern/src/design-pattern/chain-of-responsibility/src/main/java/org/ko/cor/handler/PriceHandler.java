package org.ko.cor.handler;

/**
 * 价格处理人, 负责处理客户的责任申请
 */
public abstract class PriceHandler {

    /**
     * 直接后继, 用于处理请求
     */
    protected PriceHandler successor;

    public void setSuccessor(PriceHandler successor) {
        this.successor = successor;
    }

    /**
     * 处理折扣申请
     */
    public abstract void processDiscount(float discount);

}
