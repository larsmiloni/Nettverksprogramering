import { createRouter, createWebHistory } from 'vue-router'
import CodeCompiler from '@/views/CodeCompiler.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'codeCompiler',
      component: CodeCompiler
    },
  ]
})

export default router
