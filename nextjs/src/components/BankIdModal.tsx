'use client'
import BankIdLogga from '@/assets/images/bankid.png'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Dialog, DialogContent, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import { AnimatePresence, motion } from 'framer-motion'
import { CheckCircle, Expand, Loader2, RefreshCw, Shrink, X } from 'lucide-react'
import Image from 'next/image'
import QRCode from 'react-qr-code'
import { useEffect, useRef, useState } from 'react'
import type { AuthState } from '@/lib/types/auth'
import { Spinner } from './ui/spinner'

interface BankIDModalProps {
  isOpen: boolean
  cancelPolling: () => void
  qrCode: string | null
  authState: AuthState
  setAuthState: (state: AuthState) => void
  retryLogin: () => void
  authCountdown: number
}

export const BankIDModal = ({ isOpen, cancelPolling, qrCode, authState, setAuthState, retryLogin, authCountdown }: BankIDModalProps) => {
  const [isFullscreen, setIsFullscreen] = useState(false)
  const [showExtendButton, setShowExtendButton] = useState(false)
  const [screenReaderCountdown, setScreenReaderCountdown] = useState('')
  const qrButtonRef = useRef<HTMLButtonElement>(null)
  const extendButtonRef = useRef<HTMLButtonElement>(null)
  const mainMessageRef = useRef<HTMLDivElement>(null)

  // Handle countdown and extend button visibility
  useEffect(() => {
    if (authState === 'qr-code' && authCountdown > 0) {
      // Show extend button when 30 seconds or less remaining
      setShowExtendButton(authCountdown <= 30)

      // Update screen reader announcements at key intervals
      const minutes = Math.floor(authCountdown / 60)
      const seconds = authCountdown % 60

      if (minutes > 0 && seconds === 0) {
        setScreenReaderCountdown(`${minutes} minut${minutes !== 1 ? 'er' : ''} kvar`)
      } else if (minutes === 0 && [50, 40, 30, 20, 10, 5, 4, 3, 2, 1].includes(seconds)) {
        setScreenReaderCountdown(`${seconds} sekund${seconds !== 1 ? 'er' : ''} kvar`)
      }
    } else {
      setShowExtendButton(false)
      setScreenReaderCountdown('')
    }
  }, [authCountdown, authState])

  // Handle keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (!isOpen || authState !== 'qr-code') return

      // Space bar extends time regardless of focus
      if (event.code === 'Space' && showExtendButton) {
        event.preventDefault()
        handleExtendTime()
      }

      // Escape exits fullscreen
      if (event.code === 'Escape' && isFullscreen) {
        event.preventDefault()
        exitFullscreen()
      }
    }

    document.addEventListener('keydown', handleKeyDown)
    return () => document.removeEventListener('keydown', handleKeyDown)
  }, [isOpen, authState, showExtendButton, isFullscreen])

  // Focus management when auth state changes
  useEffect(() => {
    if (isOpen && mainMessageRef.current) {
      // Small delay to ensure DOM is updated
      setTimeout(() => {
        mainMessageRef.current?.focus()
      }, 100)
    }
  }, [authState, isOpen])

  const handleCancel = () => {
    setIsFullscreen(false)
    cancelPolling()
  }

  const handleRetry = () => {
    setIsFullscreen(false)
    retryLogin()
  }

  const handleQrClick = () => {
    if (isFullscreen) {
      exitFullscreen()
    } else {
      enterFullscreen()
    }
  }

  const enterFullscreen = () => {
    setIsFullscreen(true)
    // Focus on extend button if visible, otherwise QR button
    setTimeout(() => {
      if (showExtendButton && extendButtonRef.current) {
        extendButtonRef.current.focus()
      } else if (qrButtonRef.current) {
        qrButtonRef.current.focus()
      }
    }, 100)
  }

  const exitFullscreen = () => {
    setIsFullscreen(false)
    // Focus on extend button if visible, otherwise QR button
    setTimeout(() => {
      if (showExtendButton && extendButtonRef.current) {
        extendButtonRef.current.focus()
      } else if (qrButtonRef.current) {
        qrButtonRef.current.focus()
      }
    }, 100)
  }

  const handleExtendTime = () => {
    retryLogin()
    // Focus back to QR code button after extending
    setTimeout(() => {
      qrButtonRef.current?.focus()
    }, 100)
  }

  const formatCountdownDisplay = (countdown: number) => {
    const minutes = Math.floor(countdown / 60)
    const seconds = countdown % 60
    if (minutes > 0) {
      return `${minutes}:${seconds.toString().padStart(2, '0')}`
    }
    return `${seconds}s`
  }

  return (
    <>
      <Dialog open={isOpen && !isFullscreen} onOpenChange={handleCancel}>
        <DialogContent className='sm:max-w-md'>
          <DialogHeader>
            <DialogTitle className='flex items-center space-x-4'>
              <Image src={BankIdLogga} alt='BankID Logo' width={100} height={100} className='h-8 w-auto' />
              <span>BankID-autentisering</span>
            </DialogTitle>
          </DialogHeader>

          <AnimatePresence>
            <motion.div
              key={authState}
              initial={{ opacity: 0, height: 0 }}
              animate={{ opacity: 1, height: 'auto' }}
              exit={{ opacity: 0, height: 0 }}
              transition={{ duration: 0.2, ease: 'easeInOut' }}
              layout
              className='space-y-6 overflow-hidden'
            >
              {authState === 'loading' && (
                <div className='text-center space-y-4'>
                  <div className='flex justify-center'>
                    <Spinner variant='ring' className='h-12 w-12 text-primary' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold'>Startar BankID...</h3>
                    <p className='text-sm text-gray-600'>Förbereder säker anslutning</p>
                  </div>
                </div>
              )}

              {authState === 'qr-code' && qrCode && (
                <div className='space-y-4'>
                  <div className='text-center' ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold mb-2'>Identifiera dig med BankID</h3>
                    <p className='text-sm text-gray-600 mb-4'>Öppna BankID-appen på din mobil och skanna QR-koden nedan</p>
                  </div>

                  <Card className='bg-gray-50'>
                    <CardContent className='p-6'>
                      <div className='flex justify-center mb-4'>
                        <button
                          ref={qrButtonRef}
                          onClick={handleQrClick}
                          className='focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 rounded-lg'
                          aria-label='QR-kod för BankID autentisering. Klicka för att öppna i fullskärm'
                          type='button'
                        >
                          <QRCode value={qrCode} size={200} bgColor='transparent' fgColor='black' level='H' />
                        </button>
                      </div>

                      <div className='text-center space-y-2'>
                        <div className='flex items-center justify-center space-x-2 text-sm text-gray-600'>
                          <Loader2 className='h-4 w-4 animate-spin' />
                          <span>Väntar på autentisering...</span>
                        </div>

                        {/* Visual countdown timer (hidden from screen readers) */}
                        {authCountdown > 0 && (
                          <p className='text-sm font-medium text-muted-foreground' aria-hidden='true'>
                            Tid kvar: {formatCountdownDisplay(authCountdown)}
                          </p>
                        )}

                        {/* Screen reader countdown announcements */}
                        <div aria-live='polite' aria-atomic='true' className='sr-only'>
                          {screenReaderCountdown}
                        </div>
                      </div>
                    </CardContent>
                  </Card>

                  {/* Extend button */}
                  {showExtendButton && (
                    <div className='flex justify-center'>
                      <Button ref={extendButtonRef} onClick={handleExtendTime} variant='outline' className='font-medium'>
                        <RefreshCw className='mr-2 h-4 w-4' />
                        Förläng tiden
                      </Button>
                    </div>
                  )}

                  <div className='bg-blue-50 p-4 rounded-lg'>
                    <h4 className='font-medium text-blue-900 mb-2'>Instruktioner:</h4>
                    <ol className='text-sm text-blue-800 space-y-1'>
                      <li>1. Öppna BankID-appen på din mobil</li>
                      <li>2. Välj "Skanna QR-kod"</li>
                      <li>3. Rikta kameran mot QR-koden ovan</li>
                      <li>4. Följ instruktionerna i appen</li>
                    </ol>
                    <p className='text-xs text-blue-700 mt-2'>Tips: Klicka på QR-koden för att öppna den i fullskärm</p>
                  </div>
                </div>
              )}

              {authState === 'waiting' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <Loader2 className='h-12 w-12 animate-spin text-green-600' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-green-700'>Autentisering pågår...</h3>
                    <p className='text-sm text-gray-600'>Slutför processen i BankID-appen</p>
                  </div>
                </div>
              )}

              {authState === 'success' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <CheckCircle className='h-12 w-12 text-green-600' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-green-600'>Autentisering lyckades!</h3>
                    <p className='text-sm text-gray-600'>Du omdirigeras nu till din profil...</p>
                  </div>
                </div>
              )}

              {authState === 'failed' && (
                <div className='text-center space-y-4 py-10'>
                  <div className='flex justify-center'>
                    <X className='h-12 w-12 text-red-700' />
                  </div>
                  <div ref={mainMessageRef} tabIndex={-1}>
                    <h3 className='font-semibold text-red-700'>Misslyckades att skanna QR-koden</h3>
                    <p className='text-sm text-gray-600'>{authCountdown === 0 ? 'Tiden gick ut.' : 'Något gick fel.'} Försök igen.</p>
                  </div>
                  <Button className='w-full' onClick={handleRetry}>
                    <RefreshCw className='mr-2 h-4 w-4' />
                    Försök igen
                  </Button>
                </div>
              )}

              {(authState === 'qr-code' || authState === 'failed') && (
                <div className='flex justify-center'>
                  <Button variant='outline' onClick={handleCancel}>
                    Avbryt
                  </Button>
                </div>
              )}
            </motion.div>
          </AnimatePresence>
        </DialogContent>
      </Dialog>

      {/* Fullscreen QR Code Modal */}
      {isFullscreen && authState === 'qr-code' && qrCode && (
        <div className='fixed inset-0 z-50 bg-white flex items-center justify-center p-4 overflow-auto'>
          <div className='w-full max-w-lg mx-auto text-center space-y-6'>
            <div className='flex items-center justify-between mb-6'>
              <h2 className='text-xl font-semibold'>BankID QR-kod</h2>
              <Button variant='ghost' size='sm' onClick={exitFullscreen} aria-label='Stäng fullskärm'>
                <Shrink className='h-4 w-4' />
              </Button>
            </div>

            <div className='bg-white border-2 border-gray-200 rounded-lg p-8 inline-block'>
              <button
                ref={qrButtonRef}
                onClick={exitFullscreen}
                className='focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 rounded-lg'
                aria-label='QR-kod för BankID autentisering. Klicka för att stänga fullskärm'
                type='button'
              >
                <QRCode value={qrCode} size={Math.min(300, window.innerWidth * 0.6)} bgColor='white' fgColor='black' level='H' />
              </button>
            </div>

            {/* Countdown and extend button in fullscreen */}
            <div className='space-y-4'>
              {authCountdown > 0 && (
                <p className='text-lg font-medium' aria-hidden='true'>
                  Tid kvar: {formatCountdownDisplay(authCountdown)}
                </p>
              )}

              {/* Screen reader countdown in fullscreen */}
              <div aria-live='polite' aria-atomic='true' className='sr-only'>
                {screenReaderCountdown}
              </div>

              {showExtendButton && (
                <Button ref={extendButtonRef} onClick={handleExtendTime} size='lg' className='font-medium'>
                  <RefreshCw className='mr-2 h-5 w-5' />
                  Förläng tiden
                </Button>
              )}
            </div>

            <div className='text-sm text-gray-600 max-w-md mx-auto'>
              <p>Skanna QR-koden med BankID-appen på din mobil</p>
              <p className='mt-2'>Tryck på mellanslag för att förlänga tiden när knappen visas</p>
            </div>
          </div>
        </div>
      )}
    </>
  )
}
