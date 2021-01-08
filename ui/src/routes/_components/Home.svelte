<script>
	import { onMount } from 'svelte';
	import MainView from './MainView/index.svelte';
	import * as api from 'api.js';

	export let p = 1;

	let tab;
	let tag;


	onMount(async () => {
		({ tags } = await api.get('tags'));
	});
</script>

<svelte:head>
	<title>Conduit</title>
</svelte:head>

<div class="home-page">
	<div class="container-fluid page">
		<div class="row">
			<MainView {p} {tag} bind:tab />
		</div>
	</div>
</div>