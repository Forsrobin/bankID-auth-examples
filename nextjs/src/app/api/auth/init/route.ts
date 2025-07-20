import authService from '@/server/actions/auth/auth.service'
import { NextResponse } from 'next/server'

export type InitAuthResponse = {
  orderRef: string
  authCountdown: number
}

export async function GET() {
  const result = await authService.initAuth()
  return NextResponse.json(result)
}
