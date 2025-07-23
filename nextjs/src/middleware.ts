import { NextRequest, NextResponse } from 'next/server'
import type { DecodedPayload } from './server/actions/auth/auth.service'
import authService from './server/actions/auth/auth.service'

export const ACCESS_TOKEN_EXP = '30d' // 30 days

// Helper to redirect to /auth and clear cookies
function redirectToAuthAndClearCookies(request: NextRequest, debugLog?: string) {
  const response = NextResponse.redirect(new URL('/auth', request.url))
  response.cookies.delete('accessToken')
  if (process.env.NODE_ENV === 'development' && debugLog) {
    console.error(debugLog)
  }
  return response
}

export async function middleware(request: NextRequest) {
  const accessToken = request.headers.get('Authorization')?.split(' ')[1] || request.cookies.get('accessToken')?.value

  if (!accessToken) return redirectToAuthAndClearCookies(request, 'No access token found in request')

  const data = (await authService.decodeToken(accessToken)) as DecodedPayload | null

  if (!data) return redirectToAuthAndClearCookies(request, 'Failed to decode access token')

  const decodedAccessToken = await authService.verifyToken(accessToken)

  if (decodedAccessToken instanceof Error) return redirectToAuthAndClearCookies(request, 'Invalid access token')

  if (request.nextUrl.pathname.startsWith('/auth')) return NextResponse.redirect(new URL('/', request.url))

  return NextResponse.next()
}

export const config = {
  runtime: 'nodejs',
  matcher: ['/'],
}
