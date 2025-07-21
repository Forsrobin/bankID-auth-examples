'use client'
import { BankIDModal } from '@/components/BankIdModal'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card'
import type { AuthState } from '@/lib/types/auth'
import { useMutation, useQuery } from '@tanstack/react-query'
import { setCookie } from 'cookies-next'
import { Shield } from 'lucide-react'
import { useRouter } from 'next/navigation'
import { useEffect, useRef, useState } from 'react'
import type { PoolAuthResponse } from '../api/auth/poll/route'
import type { InitAuthResponse } from '../api/auth/init/route'

export default function Auth() {
  const [showBankIDModal, setShowBankIDModal] = useState(false)
  const [qrCode, setQrCode] = useState<string | null>(null)
  const [authState, setAuthState] = useState<AuthState>('pending')
  const [authCountdown, setAuthCountdown] = useState<number>(0)
  const [orderRef, setOrderRef] = useState<string | null>(null)

  // Track the countdown timer
  const countdownRef = useRef<NodeJS.Timeout | null>(null)
  const router = useRouter()

  // Equivalent to trpc.auth.pollAuth.useQuery
  const { data: pollAuthData } = useQuery({
    queryKey: ['pollAuth', orderRef],
    queryFn: async () => {
      const res = await fetch(`/api/auth/poll?orderRef=${orderRef}`)
      if (!res.ok) throw new Error('Failed to poll auth status')
      const data = (await res.json()) as PoolAuthResponse
      return data
    },
    enabled: orderRef !== null,
    refetchInterval: 1000,
    refetchOnWindowFocus: false,
  })

  // Equivalent to trpc.auth.login.useMutation
  const authMutation = useMutation({
    mutationFn: async () => {
      const res = await fetch('/api/auth/init')
      if (!res.ok) throw new Error('Failed to init auth')
      const data = (await res.json()) as InitAuthResponse
      return data
    },
    onSuccess: (data) => {
      setAuthState('qr-code')
      setOrderRef(data.orderRef)
      setAuthCountdown(data.authCountdown)
    },
  })

  // Handle countdown timer
  useEffect(() => {
    if (authState === 'qr-code' && authCountdown > 0) {
      countdownRef.current = setInterval(() => {
        setAuthCountdown((prev) => {
          if (prev <= 1) {
            setAuthState('failed')
            resetAuthState()
            return 0
          }
          return prev - 1
        })
      }, 1000)
    } else {
      if (countdownRef.current) {
        clearInterval(countdownRef.current)
        countdownRef.current = null
      }
    }

    return () => {
      if (countdownRef.current) {
        clearInterval(countdownRef.current)
      }
    }
  }, [authState, authCountdown])

  useEffect(() => {
    if (!pollAuthData) return

    switch (pollAuthData.status) {
      case 'newOrderRef':
        if (pollAuthData.orderRef) setOrderRef(pollAuthData.orderRef)
        if (pollAuthData.qrCode) setQrCode(pollAuthData.qrCode)

        break
      case 'qrCode':
        if (authState !== 'qr-code') setAuthState('qr-code')

        if (pollAuthData.qrCode) setQrCode(pollAuthData.qrCode)

        break
      case 'complete':
        // Create a access token and set it in cookies
        // WARNING: This is a simplified example, in production you should handle this securely
        // using refresh tokens or similar mechanisms
        setCookie('accessToken', pollAuthData.token, {
          maxAge: 60 * 60 * 24 * 30,
          expires: new Date(Date.now() + 60 * 60 * 24 * 30 * 1000),
        })
        setAuthState('success')
        clearStates()
        setTimeout(() => {
          return router.push('/')
        }, 1000)
        break
      case 'failed':
        setAuthState('failed')
        clearStates()
        break
    }
  }, [authState, pollAuthData, router])

  const clearStates = () => {
    setQrCode(null)
    setOrderRef(null)
    setAuthCountdown(0)
    if (countdownRef.current) {
      clearInterval(countdownRef.current)
      countdownRef.current = null
    }
  }

  const resetAuthState = () => {
    setAuthState('loading')
    setQrCode(null)
    setOrderRef(null)
    setAuthCountdown(0)
    if (countdownRef.current) {
      clearInterval(countdownRef.current)
      countdownRef.current = null
    }
  }

  const handleBankIDLogin = () => {
    setShowBankIDModal(true)
    resetAuthState()
    authMutation.mutate()
  }

  const retryLogin = () => {
    resetAuthState()
    authMutation.reset()
    authMutation.mutate()
  }

  const cancelPolling = () => {
    resetAuthState()
    setShowBankIDModal(false)
    authMutation.reset()
  }

  // Handle cleanup on unmount
  useEffect(() => {
    return () => {
      if (countdownRef.current) {
        clearInterval(countdownRef.current)
      }
    }
  }, [])

  return (
    <div className='min-h-screen flex flex-col bg-gradient-to-br from-blue-50 via-white to-blue-50'>
      <div className='container flex-1 mx-auto px-4 py-12 flex items-center justify-center pb-32'>
        <div className='mx-auto max-w-6xl'>
          <div className='grid gap-12 lg:grid-cols-1 lg:gap-16'>
            <div className='space-y-4'>
              <h2 className='text-4xl font-bold justify-center flex items-center gap-2 tracking-tight text-gray-900'>
                <span className='block text-primary'>BankID</span>
                inloggning
              </h2>
              <p className='text-lg text-gray-600'>Simpelt demo av BankID-inloggning och autentisering</p>
            </div>

            <div className='flex items-center justify-center'>
              <Card className='w-full max-w-md shadow-xl'>
                <CardHeader className='space-y-1 text-center'>
                  <CardTitle className='text-2xl font-bold'>Logga in</CardTitle>
                  <CardDescription>Använd ditt BankID för att komma åt din förvaringsprofil</CardDescription>
                </CardHeader>
                <CardContent className='space-y-6'>
                  <div className='space-y-4'>
                    <Button isLoading={showBankIDModal} onClick={handleBankIDLogin} className='w-full bg-primary hover:bg-primary-/90' size='lg'>
                      <Shield className='mr-2 h-5 w-5' />
                      Logga in med BankID
                    </Button>

                    <div className='relative'>
                      <div className='absolute inset-0 flex items-center'>
                        <span className='w-full border-t' />
                      </div>
                      <div className='relative flex justify-center text-xs uppercase'>
                        <span className='bg-white px-2 text-gray-500'>Säker inloggning</span>
                      </div>
                    </div>

                    <div className='rounded-lg bg-blue-50 p-4'>
                      <div className='flex items-start space-x-3'>
                        <Shield className='h-5 w-5 text-primary mt-0.5' />
                        <div className='text-sm'>
                          <p className='font-medium text-primary'>Säker och trygg</p>
                          <p className='text-primary'>Vi använder BankID för att säkerställa din identitet och skydda dina uppgifter.</p>
                        </div>
                      </div>
                    </div>
                  </div>
                </CardContent>
              </Card>
            </div>
          </div>
        </div>
      </div>

      <BankIDModal
        setAuthState={setAuthState}
        authState={authState}
        isOpen={showBankIDModal}
        cancelPolling={cancelPolling}
        qrCode={qrCode}
        authCountdown={authCountdown}
        retryLogin={retryLogin}
      />
    </div>
  )
}
