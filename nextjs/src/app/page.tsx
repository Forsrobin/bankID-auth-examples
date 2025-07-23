'use client'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card'
import { useUserContext } from '@/contexts/UserContext'
import { deleteCookie } from 'cookies-next'
import { LogOut, Shield, User } from 'lucide-react'
import { useRouter } from 'next/navigation'

export default function Home() {
  const { user } = useUserContext()
  const router = useRouter()

  const logout = async () => {
    await deleteCookie('accessToken')
    router.push('/auth')
  }

  if (!user) {
    logout()
  }

  return (
    <div className='min-h-screen bg-gray-50 p-20'>
      <div className='mx-auto max-w-2xl space-y-6'>
        {/* Header */}
        <div className='text-center space-y-2'>
          <h1 className='text-3xl font-bold text-primary'>✨ Du är nu inloggad ✨</h1>
          <Badge variant='secondary' className='bg-slate-200 text-text-primary'>
            Autentiserad via BankID
          </Badge>
        </div>

        {/* Login Information Card */}
        <Card>
          <CardHeader>
            <CardTitle className='flex items-center gap-2'>
              <User className='h-5 w-5' />
              Inloggningsinformation
            </CardTitle>
            <CardDescription>Här visas information från din BankID-autentisering</CardDescription>
          </CardHeader>
          <CardContent className='space-y-4'>
            <div className='grid grid-cols-1 md:grid-cols-2 gap-4'>
              <div className='space-y-2'>
                <label className='text-sm font-medium text-gray-600'>Personnummer</label>
                <div className='p-3 bg-gray-100 rounded-md font-mono'>{user?.personalNumber}</div>
              </div>
              <div className='space-y-2'>
                <label className='text-sm font-medium text-gray-600'>Fullständigt namn</label>
                <div className='p-3 bg-gray-100 rounded-md'>{user?.name}</div>
              </div>
              <div className='space-y-2'>
                <label className='text-sm font-medium text-gray-600'>Förnamn</label>
                <div className='p-3 bg-gray-100 rounded-md'>{user?.givenName}</div>
              </div>
              <div className='space-y-2'>
                <label className='text-sm font-medium text-gray-600'>Efternamn</label>
                <div className='p-3 bg-gray-100 rounded-md'>{user?.surname}</div>
              </div>
            </div>
          </CardContent>
        </Card>

        {/* Security Information */}
        <Card className='border-amber-200 bg-amber-50'>
          <CardHeader>
            <CardTitle className='flex items-center gap-2 text-amber-800'>
              <Shield className='h-5 w-5' />
              Säkerhetsinformation
            </CardTitle>
          </CardHeader>
          <CardContent className='text-amber-900 space-y-3'>
            <p>
              <strong>Viktigt:</strong> Denna struktur använder test BankID-certifikat för utveckling och demonstration.
            </p>
            <p>När du går över till produktion är det kritiskt att tänka på säkerheten och korrekt hantering av certifikat:</p>
            <ul className='list-disc list-inside space-y-1 ml-4'>
              <li>Använd endast produktionscertifikat från BankID</li>
              <li>Förvara certifikat säkert och krypterat</li>
              <li>Implementera korrekt certifikatvalidering</li>
              <li>Använd HTTPS för all kommunikation</li>
              <li>Logga säkerhetshändelser för övervakning</li>
            </ul>
            <p className='text-sm'>Läs mer i BankID:s tekniska dokumentation och säkerhetsriktlinjer innan produktionssättning.</p>
          </CardContent>
        </Card>

        {/* Logout Button */}
        <div className='text-center'>
          <Button onClick={logout} variant='outline' size='lg' className='gap-2 bg-transparent'>
            <LogOut className='h-4 w-4' />
            Logga ut
          </Button>
        </div>
      </div>
    </div>
  )
}
