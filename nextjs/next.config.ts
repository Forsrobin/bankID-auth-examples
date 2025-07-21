import type { NextConfig } from 'next'

const nextConfig: NextConfig = {
  reactStrictMode: true,
  experimental: {
    nodeMiddleware: true,
  },
}

export default nextConfig
