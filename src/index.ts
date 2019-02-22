const { WinCerts } = process.platform === 'win32' && require('../build/Release/win-certs.node');

export const getIssuerList = () => {
    if (process.platform !== 'win32') {
        return []
    }
    const i = new WinCerts()
    const items: string[] = []
    let result = i.next()
    while (!result.done) {
        items.push(result.value)
        result = i.next()
    }
    return items
}
