import type { NextConfig } from 'next'

const nextConfig: NextConfig = {
  reactStrictMode: true,
  experimental: {
    useCache: true,
    nodeMiddleware: true,
  },
}

export default nextConfig
