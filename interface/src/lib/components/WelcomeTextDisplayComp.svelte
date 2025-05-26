<script lang="ts">
    import { getJsonRest } from '$lib/stores/rest';
    import Spinner from './Spinner.svelte';

    let welcomeText = '';

    async function getWelcomeText() {
        return getJsonRest('rest/spin-seq/welcome', {text: welcomeText}).then((data) => {
            welcomeText = data.text;
            return data;
        });
    }
    getWelcomeText();
</script>

{#if welcomeText}
<div class="rounded-lg bg-base-200 p-6 shadow-lg max-w-2xl w-full self-center shadow-lg">
    <div class="prose max-w-none">
        <p class="whitespace-pre-wrap text-lg">{welcomeText || ''}</p>
    </div>
</div>
{/if}