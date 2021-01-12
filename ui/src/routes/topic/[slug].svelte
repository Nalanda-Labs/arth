<script context="module">
	import * as api from 'api.js';

	export async function preload({ params }, { user }) {
		if (!user) {
			this.redirect(302, `/login`);
		}

		const { slug } = params;
		const { topic } = await api.get(`topic/${slug}`, null);
		return { topic, slug };
	}
</script>

<script>
	import Editor from './_Editor.svelte';

	export let slug;
	export let topic;
</script>

<Editor {topic} {slug}/>