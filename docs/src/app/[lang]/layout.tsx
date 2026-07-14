import { RootProvider } from 'fumadocs-ui/provider/next';
import '../global.css';
import { Inter } from 'next/font/google';
import { i18nProvider } from '@/lib/layout.shared';

const inter = Inter({
  subsets: ['latin', 'cyrillic'],
});

export default async function Layout({ params, children }: LayoutProps<'/[lang]'>) {
  const { lang } = await params;

  return (
    <html lang={lang} className={inter.className} suppressHydrationWarning>
      <body className="flex flex-col min-h-screen">
        <RootProvider i18n={i18nProvider(lang)}>{children}</RootProvider>
      </body>
    </html>
  );
}
