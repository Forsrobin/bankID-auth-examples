'use client'
import { BankIDModal } from '@/components/BankIdModal'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card'
import type { AuthState } from '@/lib/types/auth'
import { useMutation, useQuery } from '@tanstack/react-query'
import { Building2, Clock, Shield, Users } from 'lucide-react'
import { useRouter } from 'next/navigation'
import { useEffect, useRef, useState } from 'react'

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
      return await res.json()
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
      return await res.json()
    },
    onSuccess: (data) => {
      setAuthState('qr-code')
      setAuthCountdown(data.authCountdown)
      setOrderRef(data.orderRef)
    },
    onError: () => {
      setAuthState('failed')
      clearStates()
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
        if (pollAuthData.orderRef) {
          setOrderRef(pollAuthData.orderRef)
        }
        if (pollAuthData.qrCode) {
          setQrCode(pollAuthData.qrCode)
        }
        break
      case 'qrCode':
        if (authState !== 'qr-code') {
          setAuthState('qr-code')
        }
        if (pollAuthData.qrCode) {
          setQrCode(pollAuthData.qrCode)
        }
        break
      case 'complete':
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
  }, [pollAuthData, authState, router])

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
          <div className='grid gap-12 lg:grid-cols-2 lg:gap-16'>
            <div className='flex flex-col justify-center space-y-8'>
              <div className='space-y-4'>
                <h2 className='text-4xl font-bold tracking-tight text-gray-900'>
                  Välkommen till
                  <span className='block text-blue-600'>Förvaringsportalen</span>
                </h2>
                <p className='text-lg text-gray-600'>Din säkra plattform för förvaring och hantering av viktiga dokument och tillgångar.</p>
              </div>

              <div className='grid gap-4 sm:grid-cols-2'>
                <div className='flex items-start space-x-3'>
                  <div className='flex h-8 w-8 items-center justify-center rounded-lg bg-green-100'>
                    <Shield className='h-4 w-4 text-green-600' />
                  </div>
                  <div>
                    <h3 className='font-semibold text-gray-900'>Säker autentisering</h3>
                    <p className='text-sm text-gray-600'>BankID-integration för maximal säkerhet</p>
                  </div>
                </div>
                <div className='flex items-start space-x-3'>
                  <div className='flex h-8 w-8 items-center justify-center rounded-lg bg-blue-100'>
                    <Users className='h-4 w-4 text-blue-600' />
                  </div>
                  <div>
                    <h3 className='font-semibold text-gray-900'>Användarcentrerat</h3>
                    <p className='text-sm text-gray-600'>Enkel och intuitiv användarupplevelse</p>
                  </div>
                </div>
                <div className='flex items-start space-x-3'>
                  <div className='flex h-8 w-8 items-center justify-center rounded-lg bg-purple-100'>
                    <Clock className='h-4 w-4 text-purple-600' />
                  </div>
                  <div>
                    <h3 className='font-semibold text-gray-900'>Tillgänglig 24/7</h3>
                    <p className='text-sm text-gray-600'>Åtkomst till dina tillgångar när som helst</p>
                  </div>
                </div>
                <div className='flex items-start space-x-3'>
                  <div className='flex h-8 w-8 items-center justify-center rounded-lg bg-orange-100'>
                    <Building2 className='h-4 w-4 text-orange-600' />
                  </div>
                  <div>
                    <h3 className='font-semibold text-gray-900'>Professionell service</h3>
                    <p className='text-sm text-gray-600'>Experthjälp och support</p>
                  </div>
                </div>
              </div>
            </div>

            <div className='flex items-center justify-center'>
              <Card className='w-full max-w-md shadow-xl'>
                <CardHeader className='space-y-1 text-center'>
                  <CardTitle className='text-2xl font-bold'>Logga in</CardTitle>
                  <CardDescription>Använd ditt BankID för att komma åt din förvaringsprofil</CardDescription>
                </CardHeader>
                <CardContent className='space-y-6'>
                  <div className='space-y-4'>
                    <Button isLoading={showBankIDModal} onClick={handleBankIDLogin} className='w-full bg-blue-600 hover:bg-blue-700' size='lg'>
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
                        <Shield className='h-5 w-5 text-blue-600 mt-0.5' />
                        <div className='text-sm'>
                          <p className='font-medium text-blue-900'>Säker och trygg</p>
                          <p className='text-blue-700'>Vi använder BankID för att säkerställa din identitet och skydda dina uppgifter.</p>
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
