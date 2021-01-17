<script>
	export let response;
</script>

<div style="margin-top:10px">
{#if response && response.error}
	<div class="alert alert-danger alert-dismissible fade show" role="alert">
		{response.error}
		<button
			type="button"
			class="btn-close"
			data-bs-dismiss="alert"
			aria-label="Close" />
	</div>
{/if}

{#if response && response.message}
	<div class="alert alert-danger alert-dismissible fade show" role="alert">
		{response.error}
		<button
			type="button"
			class="btn-close"
			data-bs-dismiss="alert"
			aria-label="Close" />
	</div>
{/if}
</div>