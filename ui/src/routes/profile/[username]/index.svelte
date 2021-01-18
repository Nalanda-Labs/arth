<script context="module">
	export function preload({ params }) {
		let username = params.username;
		return {username};
	}
</script>

<script>
	import { navOptions } from "./_Tabs.svelte"; // import application navigation
	let selected = navOptions[0]; // keep track of the selected 'page' object (default to the about component since we must have local db connection established first)
	let intSelected = 0; // selected page index

	export let username;
	// change the selected component (the event.originalTarget.id is not accessible in Chrome so switched to event.srcElement.id)
	function changeComponent(event) {
		selected = navOptions[event.srcElement.id];
		intSelected = event.srcElement.id;
	}
</script>

<svelte:head>
	<title>All about {username}</title>
</svelte:head>

<div>
	<!--app navigation -->
	<h3 style="margin-top:20px">{username}'s Profile</h3>
	<hr />
	<ul class="nav nav-tabs">
		{#each navOptions as option, i}
			<li class="nav-item" style="margin-right:25px;">
				<a
					class={intSelected == i ? 'nav-link active p-2 ml-1' : 'p-2 ml-1 nav-link'}
					on:click={changeComponent}
					id={i}
					role="tab">
					{option.page}
				</a>
			</li>
		{/each}
	</ul>
	<!-- content wrapper -->
	<div class="row" style="margin-top:10px">
		<div class="col-sm-12 col-md-12">
		<svelte:component this={selected.component} {username}/>
		</div>
	</div>
</div>
