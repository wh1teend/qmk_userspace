import type { BaseLayoutProps } from 'fumadocs-ui/layouts/shared';
import { defineI18nUI } from 'fumadocs-ui/i18n';
import { i18n } from './i18n';

const i18nUI = defineI18nUI(i18n, {
  en: {
    displayName: 'English',
  },
  ru: {
    displayName: 'Русский',
    search: 'Поиск',
    searchNoResult: 'Ничего не найдено',
    toc: 'На этой странице',
    tocNoHeadings: 'Нет заголовков',
    lastUpdate: 'Обновлено',
    chooseLanguage: 'Выберите язык',
    nextPage: 'Далее',
    previousPage: 'Назад',
    chooseTheme: 'Тема',
    editOnGithub: 'Править на GitHub',
  },
});

export const i18nProvider = i18nUI.provider;

export function baseOptions(): BaseLayoutProps {
  return {
    nav: {
      enabled: false,
    },
    themeSwitch: {
      enabled: false,
    },
  };
}
