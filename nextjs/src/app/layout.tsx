import type { Metadata } from 'next'
import { Roboto_Flex } from 'next/font/google'
import './globals.css'
import { QueryProvider } from '@/components/QueryProvider'

const roboto = Roboto_Flex({
  variable: '--font-roboto',
  subsets: ['latin'],
})

export const metadata: Metadata = {
  title: 'BankID with c++ backend',
  description: 'A BankID authentication example using React and C++',
}

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode
}>) {
  return (
    <html lang='en'>
      <body className={`${roboto.className} antialiased`}>
        <QueryProvider>{children}</QueryProvider>
      </body>
    </html>
  )
}
