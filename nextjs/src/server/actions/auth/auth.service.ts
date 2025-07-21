import type { InitAuthResponse } from '@/app/api/auth/init/route'
import type { PoolAuthResponse } from '@/app/api/auth/poll/route'
import { bankIdClient } from '@/server/lib/bankid'
import env from '@/server/env'
import type { ServiceAsyncReturnType } from '@/lib/types/service'
import { QrGenerator } from 'bankid'
import { decodeJwt, jwtVerify, SignJWT } from 'jose'
import { ACCESS_TOKEN_EXP } from '../../../middleware'

const AUTH_TIMEOUT = 300 // seconds, 5 minutes

export type DecodedPayload = {
  personalNumber: string
}

const authService = {
  initAuth: async (): Promise<InitAuthResponse> => {
    try {
      const { orderRef, qr } = await bankIdClient.authenticate({
        endUserIp: '127.0.0.1',
      })

      if (!qr) throw new Error('QR code generator not found')

      return {
        orderRef,
        authCountdown: AUTH_TIMEOUT,
      }
    } catch (error) {
      console.error('Error initializing auth:', error)
      throw new Error('Failed to initialize auth')
    }
  },

  pollAuth: async (orderRef: string): Promise<PoolAuthResponse> => {
    const qrCacheEntry = await QrGenerator.latestQrFromCache(orderRef)

    const invalid: PoolAuthResponse = {
      orderRef: null,
      qrCode: null,
      status: 'failed',
      token: null,
    }

    if (!qrCacheEntry) {
      console.log('No QR code found in cache for orderRef:', orderRef)
      return invalid
    }

    const resp = await bankIdClient.collect({ orderRef })

    if (resp.status === 'complete' && resp.completionData?.user) {
      try {
        const userData = resp.completionData.user
        const accessToken = await authService.generateAccessToken(userData.personalNumber)

        return {
          orderRef,
          qrCode: null,
          status: resp.status,
          token: accessToken,
        }
      } catch (error) {
        return invalid
      }
    } else if (resp.status === 'failed') {
      // Here we want to create a new order becuase bankID only have a 30 second validity time
      const { orderRef: newOrderRef, qr } = await bankIdClient.authenticate({
        endUserIp: '127.0.0.1',
      })

      if (!qr) return invalid

      if (newOrderRef !== orderRef) {
        const newOrderFirstQrCode = await QrGenerator.latestQrFromCache(newOrderRef)

        if (!newOrderFirstQrCode) return invalid

        return {
          status: 'newOrderRef',
          orderRef: newOrderRef,
          qrCode: newOrderFirstQrCode,
          token: null,
        }
      }

      return invalid
    }

    const newQrCode = await QrGenerator.latestQrFromCache(orderRef)

    if (!newQrCode) {
      return invalid
    }

    return {
      qrCode: newQrCode,
      status: 'qrCode',
      orderRef,
      token: null,
    }
  },

  generateAccessToken: async (personalNumber: string): Promise<string> => {
    const secret = new TextEncoder().encode(env.JWT_SECRET)
    const accessTokenPayload: DecodedPayload = { personalNumber }

    return await new SignJWT(accessTokenPayload).setProtectedHeader({ alg: 'HS512' }).setExpirationTime(ACCESS_TOKEN_EXP).sign(secret)
  },

  verifyToken: async (token: string): Promise<DecodedPayload | Error> => {
    const secret = new TextEncoder().encode(env.JWT_SECRET)

    try {
      const { payload } = await jwtVerify(token, secret)
      return payload as DecodedPayload
    } catch (error) {
      if (error instanceof Error && error.name === 'JWTExpired') {
        return new Error('Token expired')
      }
      return new Error('Invalid token')
    }
  },
  decodeToken: async (token: string): Promise<DecodedPayload> => {
    return decodeJwt(token) as DecodedPayload
  },
}

export type AuthServiceReturnTypes = ServiceAsyncReturnType<typeof authService>
export default authService
