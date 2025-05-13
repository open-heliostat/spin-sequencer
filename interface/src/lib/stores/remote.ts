import { getJsonRest, postJsonRest } from "./rest";

export async function getJsonRestWithCanFallback<T>(path: string, data: T, ip: string, rxId: number): Promise<T> {
    let fullPath = ip ? "http://" + ip + "/rest" + path : "/rest/can/tun/" + rxId + path;
    return getJsonRest(fullPath, data, {signal: AbortSignal.timeout(1000)}).then((result) => {
        data = result;
        return data;
    }).catch((error) => {
        if (ip && rxId) {
            return getJsonRest("/rest/can/tun/" + rxId + path, data, {signal: AbortSignal.timeout(2000)}).then((result) => {
                data = result;
                return data;
            });
        }
        return data;
    });
}

export async function postJsonRestWithCanFallback<T>(path: string, data: T, ip: string, rxId: number): Promise<T> {
    let fullPath = ip ? "http://" + ip + "/rest" + path : "/rest/can/tun/" + rxId + path;
    return postJsonRest(fullPath, data, {signal: AbortSignal.timeout(1000)}).then((result) => {
        data = result;
        return data;
    }).catch((error) => {
        if (ip && rxId) {
            return postJsonRest("/rest/can/tun/" + rxId + path, data, {signal: AbortSignal.timeout(2000)}).then((result) => {
                data = result;
                return data;
            });
        }
        return data;
    });
}