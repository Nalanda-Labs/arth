<script context="module">
  export function preload({ params }, { user }) {
    if (!user) {
      this.redirect(302, `/login`);
    }
    let [id, slug] = [params.id, params.slug];
    return { id, slug };
  }
</script>

<script>
  import Editor from "./_Editor.svelte";
  export let id;
  export let slug;
  let topic = { title: "", body: "", tagList: [] };
</script>

<svelte:head>
  <title>Edit Topic • Arth</title>
</svelte:head>

<Editor {topic} {slug} {id}/>

<style>
  @media (max-width: 720px) {
    .tags {
      width: 100%;
    }
  }
  @media (max-width: 4096px) {
    .tags {
      width: 800px;
    }
  }
</style>
