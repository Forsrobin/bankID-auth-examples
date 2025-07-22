type BankIDRequest = {
  endUserIp: string // REQUIRED – external IP address
  returnRisk?: boolean
  returnUrl?: string
  userNonVisibleData?: string // base64-encoded
  userVisibleData?: string // base64-encoded UTF-8
  userVisibleDataFormat?: 'plaintext' | 'simpleMarkdownV1'
  app?: {
    appIdentifier: string // Android package or iOS bundle ID
    deviceOS: string
    deviceIdentifier: string // Unique, persistent device ID
    deviceModelName: string
  }
  web?: {
    deviceIdentifier: string // Unique browser/device identifier
    referringDomain: string // Punycode if IDN
    userAgent: string
  }

  requirement?: {
    cardReader: 'class1' | 'class2'
    certificatePolicies: string[] // e.g. "1.2.752.78.1.1"
    mrtd: boolean
    personalNumber: string
    pinCode: boolean
  }
}
