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

<svelte:head>
	<title>Create Topic • Arth</title>
</svelte:head>

<Editor {topic}/>