<script context="module">
	export function preload({ params }, { user }) {
		if (!user) {
			this.redirect(302, `/login`);
		}
	}
</script>

<script>
	import Editor from './_Editor.svelte';
	let topic = { title: '', body: '', tagList: [] };
</script>
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
<svelte:head>
	<title>Create Topic • Arth</title>
</svelte:head>

<Editor {topic}/>