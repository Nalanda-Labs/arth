<script>
	export let response;
</script>

{#if response.error}
	<ul class="error-messages">
		{#each Object.keys(response) as key}
			<li>{key} {response[key]}</li>
		{/each}
	</ul>
{/if}