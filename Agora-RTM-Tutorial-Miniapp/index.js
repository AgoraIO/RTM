/*!
 * iny-bus.js v1.1.0
 * (c) 2019-2020 landluck
 * Released under the MIT License.
 */
'use strict';

/**
 * 创建唯一id
 */
function createUid() {
    return Math.random()
        .toString()
        .substr(2);
}
function once(fn) {
    var called = false;
    return function () {
        var args = [];
        for (var _i = 0; _i < arguments.length; _i++) {
            args[_i] = arguments[_i];
        }
        if (!called) {
            called = true;
            fn.apply(this, args);
        }
    };
}

var EventBus = /** @class */ (function () {
    function EventBus() {
        /**
         * 储存事件的容器
         */
        this.events = [];
    }
    /**
     * on 新增事件监听
     * @param name 事件名
     * @param execute 回调函数
     * @param ctx 上下文 this
     * @returns { string } eventId 事件ID，用户取消该事件监听
     */
    EventBus.prototype.on = function (name, execute, ctx) {
        return this.addEvent(name, execute, ctx);
    };
    /**
     * one 只允许添加一次事件监听
     * @param name 事件名
     * @param execute 回调函数
     * @param ctx 上下文 this
     * @returns { string } eventId 事件ID，用户取消该事件监听
     */
    EventBus.prototype.once = function (name, execute, ctx) {
        return this.addEvent(name, once(execute), ctx);
    };
    /**
     * remove 移除事件监听
     * @param name 事件名
     * @param eventId 移除单个事件监听需传入
     * @returns { EventBus } EventBus EventBus 实例
     */
    EventBus.prototype.remove = function (name, eventId) {
        var events = this.events;
        var index = events.findIndex(function (event) { return event.name === name; });
        if (index === -1) {
            return this;
        }
        if (!eventId) {
            events.splice(index, 1);
            return this;
        }
        var executeIndex = events[index].executes.findIndex(function (item) { return item.id === eventId; });
        if (executeIndex !== -1) {
            events[index].executes.splice(executeIndex, 1);
        }
        return this;
    };
    /**
     * emit 派发事件
     * @param name 事件名
     * @param args 其余参数
     * @returns { EventBus } EventBus EventBus 实例
     */
    EventBus.prototype.emit = function (name) {
        var args = [];
        for (var _i = 1; _i < arguments.length; _i++) {
            args[_i - 1] = arguments[_i];
        }
        var event = this.find(name);
        if (!event) {
            return this;
        }
        var funcs = event.executes;
        funcs.forEach(function (func) {
            if (func.ctx) {
                return func.execute.apply(func.ctx, args);
            }
            func.execute.apply(func, args);
        });
        return this;
    };
    /**
     * 查找事件的方法
     * @param name
     */
    EventBus.prototype.find = function (name) {
        var events = this.events;
        for (var i = 0; i < events.length; i++) {
            if (name === events[i].name) {
                return events[i];
            }
        }
        return null;
    };
    EventBus.prototype.clear = function () {
        this.events.length = 0;
        return this;
    };
    /**
     * 添加事件的方法
     * @param name
     * @param execute
     */
    EventBus.prototype.addEvent = function (name, execute, ctx) {
        var eventId = createUid();
        var events = this.events;
        var event = this.find(name);
        if (event !== null) {
            event.executes.push({ id: eventId, execute: execute, ctx: ctx });
            return eventId;
        }
        events.push({
            name: name,
            executes: [
                {
                    id: eventId,
                    execute: execute,
                    ctx: ctx
                }
            ]
        });
        return eventId;
    };
    return EventBus;
}());

function createInstance() {
    var bus = new EventBus();
    // @ts-ignore
    return bus;
}
var bus = createInstance();
bus.create = function create() {
    return createInstance();
};

function isAliPay() {
    // @ts-ignore
    return typeof my !== 'undefined' && my.getSystemInfo;
}

var PAGE_LIFE_TIMES = {
    onLoad: 'onLoad',
    onUnload: 'onUnload'
};
var APP_LIFE_TIMES = {
    onLoad: 'onLaunch',
    onUnload: ''
};
var COMPONENT_LIFE_TIMES = isAliPay()
    ? {
        onLoad: 'onInit',
        onUnload: 'didUnmount'
    }
    : {
        onLoad: 'created',
        onUnload: 'detached'
    };

function verifyEvents(busEvents) {
    if (!busEvents) {
        return false;
    }
    return true;
}
function onLoad(ctx, onLoad) {
    var func = ctx[onLoad];
    ctx[onLoad] = function (options) {
        var ids = addEvent(ctx.busEvents, this);
        ctx.__inyEventIds = ids;
        func && func.call(this, options);
    };
}
function onUnload(ctx, onUnload) {
    if (!onUnload) {
        return;
    }
    var func = ctx[onUnload];
    ctx[onUnload] = function () {
        ctx.__inyEventIds.forEach(function (event) { return bus.remove(event.name, event.id); });
        ctx.__inyEventIds = undefined;
        func && func.call(this);
    };
}
function addEvent(events, ctx) {
    return Object.keys(events).map(function (name) {
        var event = events[name];
        if (typeof event === 'function') {
            return { id: bus.on(name, event, ctx), name: name };
        }
        if (event.once) {
            return { id: bus.once(name, event.handler, ctx), name: name };
        }
        return { id: bus.on(name, event.handler, ctx), name: name };
    });
}

function inyApp(ctx) {
    var busEvents = ctx.busEvents;
    if (!verifyEvents(busEvents)) {
        return ctx;
    }
    onLoad(ctx, APP_LIFE_TIMES.onLoad);
    onUnload(ctx, APP_LIFE_TIMES.onUnload);
    return ctx;
}

function InyPage(ctx) {
    var busEvents = ctx.busEvents;
    if (!verifyEvents(busEvents)) {
        return ctx;
    }
    onLoad(ctx, PAGE_LIFE_TIMES.onLoad);
    onUnload(ctx, PAGE_LIFE_TIMES.onUnload);
    return ctx;
}

function InyComponents(ctx) {
    var busEvents = ctx.busEvents;
    if (!verifyEvents(busEvents)) {
        return ctx;
    }
    onLoad(ctx, COMPONENT_LIFE_TIMES.onLoad);
    onUnload(ctx, COMPONENT_LIFE_TIMES.onUnload);
    return ctx;
}

bus.app = inyApp;
bus.page = InyPage;
bus.component = InyComponents;

module.exports = bus;
