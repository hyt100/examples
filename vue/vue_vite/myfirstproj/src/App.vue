<script setup>
  import { reactive, ref } from 'vue';
  import Header from "./components/header.vue";
  import Footer from './components/footer.vue';

  const web = reactive({
    show: true,
    title: 'hello vue'
  })

  const toggle = () => {
    web.show = !web.show
  }
</script>

<template>
  <Header name="erick"  />
  <h3>{{ web.title }}</h3>
  <p v-show="web.show">www.baidu.com</p>

  <button @click="toggle">点击</button>
  <Footer/>
</template>

<style scoped>

</style>
