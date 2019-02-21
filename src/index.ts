var { WinCerts } = require('bindings')('win-certs');

export const getIssuerList = () => {
    const i = new WinCerts()
    const items: string[] = []
    let result = i.next()
    while (!result.done) {
        items.push(result.value)
        result = i.next()
    }
    return items
}
