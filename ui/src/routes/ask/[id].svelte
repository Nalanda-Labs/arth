<script context="module">
	import * as api from 'api.js';

	export async function preload({ params }, { user }) {
		if (!user) {
			this.redirect(302, `/login`);
		}

		const { id } = params;
		const { topic } = await api.get(`topic/${id}`, null);
		return { topic, id };
	}
</script>

<script>
	import Editor from './_Editor.svelte';

	export let id;
	export let topic;
</script>

<Editor {topic} {id}/>