import type { Metadata } from 'next'
import { Roboto_Flex } from 'next/font/google'
import './globals.css'
import { QueryProvider } from '@/components/QueryProvider'
import { UserProvider } from '@/contexts/UserContext'

const roboto = Roboto_Flex({
  variable: '--font-roboto',
  subsets: ['latin'],
})

export const metadata: Metadata = {
  title: 'BankID - NextJS',
  description: 'A BankID authentication example using Next.js app router',
}

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode
}>) {
  return (
    <html lang='en'>
      <body className={`${roboto.className} antialiased`} suppressHydrationWarning>
        <UserProvider>
          <QueryProvider>{children}</QueryProvider>
        </UserProvider>
      </body>
    </html>
  )
}
