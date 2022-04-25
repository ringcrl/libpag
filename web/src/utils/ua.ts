const nav = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.127 Safari/537.36';
export const ANDROID = /android|adr/i.test(nav);
export const MOBILE = /(mobile)/i.test(nav) && ANDROID;
export const MACOS = !(/(mobile)/i.test(nav) || MOBILE) && /Mac OS X/i.test(nav);
export const IPHONE = /(iphone|ipad|ipod)/i.test(nav);
export const IS_WECHAT = /MicroMessenger/i.test(nav);
